
#include "flash_store.h"
#include "list.h"
#include "app_connector.h"

/*
 * dsPIC33F simulator seems to have bug with holding buffers:
 *   7th bit in table write operation offset is IGNORED both
 *   while filling buffers and while RTSP is running
 *
 * !!! THIS BUG DAMAGES CODE WHILE EXECUTING IN SIMULATOR !!!
 */

#if APP_USE_RTSP

#define FLASH_PAGE_MASK   0xFC00     // Gets 512-instruction aligned offset (1024)
#define FLASH_PAGE        0x0400     // Flash page size
#define FLASH_OFFSET_MASK 0x03FF
#define FLASH_ROW_MASK    0x007F
#define FLASH_ROW         0x0080

// Use special section defined in the custom linker script to store RTSP-relevant
// functions. Section has to be page-aligned and contain nothing but the code
// marked for this section.
#define SECURE __attribute__((section(".fixed")))

typedef struct tagFLASHOP {
    unsigned char pageNum;
    unsigned int  offset;
    uint16_t      value;
} _FlashOp;

typedef struct tagINSTRUCTION {
    uint8_t  highByte;
    uint16_t lowWord;
} _Instruction;

_PERSISTENT _ListHandle  flashOps;

// Temporary store for page
uint16_t pageStore[512] __attribute__((space(prog),aligned(2048)));
_PERSISTENT _Instruction _flash_tmp_instructionHold[64];

void _flashop_free(_FlashOp *op);
_FlashOp *_flashop_copy(const _FlashOp *op);

_ListHandle _flashop_list_create() {
    _ListHandle h = list_create();
    list_set_free(h, (void (*)(void *))_flashop_free);
    list_set_copy(h, (void *(*)(void * const))_flashop_copy);
    return h;
}

void flash_init() {
    if(reset_is_cold()) {
        flashOps = _flashop_list_create();
    }
}

void SECURE _flash_start() {
    asm volatile(
        "MOV #0x55,W0            \n"   // Unlock sequence
        "MOV W0, NVMKEY          \n"
        "MOV #0xAA,W0            \n"
        "MOV W0, NVMKEY          \n"
        "BSET NVMCON,#15         \n"   // Initiate programming cycle (15=WRbit)
        "NOP                     \n"
        "NOP                     \n"
    );
}

SECURE _Bool _flash_ok() {
    return !NVMCONbits.WRERR;
}

void SECURE _flash_preconf_row() {
    NVMCONbits.WREN = 1;        // Enable write
    NVMCONbits.ERASE = 0;       // Don't erase
    NVMCONbits.NVMOP = 0b0001;  // Single row write
}

void SECURE _flash_preconf_erase() {
    NVMCONbits.WREN = 1;        // Enable write
    NVMCONbits.ERASE = 1;       // Erase
    NVMCONbits.NVMOP = 0b0010;  // Single page erase
}

void flash_set(unsigned char page, unsigned int offset, uint16_t value) {
    _FlashOp *operation = gc_malloc(sizeof(_FlashOp));
    operation->pageNum = page;
    operation->offset  = offset;
    operation->value   = value;
    list_push_back(flashOps, operation);
}

/* Returns 0 if successful */
int SECURE _flash_program_row() {
    _flash_preconf_row();   // Configure flashing operation
    _flash_start();         // Flash
    return !_flash_ok();
}

/* Returns 0 if successful */
int SECURE _flash_erase_page() {
    _flash_preconf_erase();   // Configure flashing operation
    _flash_start();           // Flash
    return !_flash_ok();
}

int SECURE flash_erase_page(unsigned char page, unsigned int offset) {
    TBLPAG = page;
    __builtin_tblwtl(offset, 0);
    return _flash_erase_page();
}

int SECURE flash_writerow(unsigned char page, unsigned int offset,
        _Instruction *row) {
    int i;
    TBLPAG = page;
    
    // Write 64 instructions == 1 row
    for(i = 0; i < 128; i+=2) {
        __builtin_tblwtl(offset + i, row[i/2].lowWord);
        __builtin_tblwth(offset + i, row[i/2].highByte);
    }
    // Flash row
    return _flash_program_row();
}

void SECURE flash_readrow(unsigned char page, unsigned int offset,
        _Instruction *row) {
    int i;
    TBLPAG = page;
    // Read 64 instructions == 1 row
    for(i = 0; i < 128; i+=2) {
        row[i/2].lowWord  = __builtin_tblrdl(offset + i);
        row[i/2].highByte = __builtin_tblrdh(offset + i);
    }
}

int flash_store_erase() {
    return flash_erase_page(FLASH_GETPAGE(pageStore),
            FLASH_GETOFFSET(pageStore));
}

void SECURE flash_store_readrow(unsigned int rowNum, _Instruction *row) {
    flash_readrow(FLASH_GETPAGE(pageStore),
                FLASH_GETOFFSET(pageStore) + rowNum*128, row);
}

int flash_store_writerow(unsigned int rowNum, _Instruction *row) {
    return flash_writerow(FLASH_GETPAGE(pageStore),
                FLASH_GETOFFSET(pageStore) + rowNum*128, row);
}

_Bool on_same_page(const _FlashOp *op1, const _FlashOp *op2) {
    return op1 && op2 && op1->pageNum == op2->pageNum &&
            (op1->offset ^ op2->offset) < FLASH_PAGE;
}

_Bool in_same_row(const _FlashOp *op1, const _FlashOp *op2) {
    return op1 && op2 && op1->pageNum == op2->pageNum &&
            (op1->offset ^ op2->offset) < FLASH_ROW;
}

void flash_readpage(unsigned char page, unsigned int offset) {
    int i;
    SYSHANDLE hp;
    
    _Instruction *progRow = _flash_tmp_instructionHold;

    offset &= FLASH_PAGE_MASK;

    // Prepare pageStor
    flash_store_erase();

    hp = high_priority_enter();
        // Read page
        for(i = 0; i < 8; ++i) {
            flash_readrow(page, offset+i*128, progRow);
            flash_store_writerow(i, progRow);
        }
    high_priority_exit(hp);
}

/* Return 0 on success and 1 if flashing error has occured */
int SECURE flash_writepage(_ListHandle pageGroup) {
    int i, k = 0, opsSize, result = 0;
    _ListIterator j;
    _FlashOp op = *(_FlashOp*)list_front(pageGroup),
            *ops;
    _Instruction *row = _flash_tmp_instructionHold;
    
    unsigned int offset;
    SYSHANDLE hp;


    // While we will be rewriting page, some parts of program may become invalid
    // Ensure we are not erasing this exact function
    if(FLASH_GETPAGE(flash_writepage) == op.pageNum &&
       (FLASH_GETOFFSET(flash_writepage) ^ op.offset) < FLASH_PAGE)
        return 2;

    // Preload list of changes
    j = list_begin(pageGroup);
    do {
        ++k;
    } while(list_iterate_fwd(j));
    list_iterator_free(j);

    ops = gc_malloc(sizeof(_FlashOp)*k);

    k = 0;
    j = list_begin(pageGroup);
    do {
        ops[k++] = *(_FlashOp*) list_iterator_value(j);
    } while(list_iterate_fwd(j));
    list_iterator_free(j);

    opsSize = k;


    hp = high_priority_enter();
        // Prepare page
        flash_erase_page(op.pageNum, op.offset);
        offset = op.offset & FLASH_PAGE_MASK;

        // Group by row
        for(i = 0; i < 8; ++i) {
            // Load row
            flash_store_readrow(i, row);

            // Apply all operations from that row
            for(k = 0; k < opsSize; ++k)
                if((ops[k].offset & FLASH_OFFSET_MASK) / 128 == i)
                    row[(ops[k].offset & FLASH_ROW_MASK) / 2].lowWord = ops[k].value;

            // Store row
            if(flash_writerow(op.pageNum, offset+i*128, row)) {
                result = 1;
                break;
            }
        }
    high_priority_exit(hp);
    gc_free(ops);

    return result;
}

int flash_write() {
    _ListHandle group = _flashop_list_create();
    _ListIterator i;
    _FlashOp *op;
    int result = 0;
    
    SYSHANDLE hp;

    led_on();

    // Group all queued writes by page
    hp = high_priority_enter();
        while(!list_is_empty(flashOps)) {
            // Add first operation
            op = _flashop_copy(list_front(flashOps));
            list_pop_front(flashOps);
            list_push_back(group, op);

            // Add all operations from the same page
            i = list_begin(flashOps);
            do {
                if(on_same_page(list_iterator_value(i), list_front(group))) {
                    op = _flashop_copy(list_iterator_value(i));
                    list_remove(i);
                    list_push_back(group, op);
                }
            } while(list_iterate_fwd(i));
            list_iterator_free(i);


            op = list_front(group);
            flash_readpage(op->pageNum, op->offset); // prepare page
            // Apply all operations from the group and program device
            if(flash_writepage(group)) {
                return 1;
            }

            // Release resources
            list_clear(group);
        }
    high_priority_exit(hp);

    list_free(group);

    led_off();

    return result;
}

void _flashop_free(_FlashOp *op) {
    gc_free(op);
}

_FlashOp *_flashop_copy(const _FlashOp *op) {
    _FlashOp *newOp = gc_malloc(sizeof(_FlashOp));
    *newOp = *op;
    return newOp;
}

#else

void flash_init() {}
void flash_set(unsigned char page, unsigned int offset, uint16_t value) {}
void flash_set_data(unsigned char page, unsigned int startOffset,
        size_t size, unsigned int *data) {}
int flash_write() {}

#endif
