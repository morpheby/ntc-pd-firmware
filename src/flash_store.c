
#include "flash_store.h"
#include "list.h"
#include <libpic30.h>

/*
 * dsPIC33F simulator seems to have bug with holding buffers:
 *   7th bit in table write operation offset is IGNORED both
 *   while filling buffers and while RTSP is running
 *
 * !!! THIS BUG DAMAGES CODE WHILE EXECUTING IN SIMULATOR !!!
 */

#if !APP_USE_RTSP && APP_USE_BOOTLOADER
#error "RTSP has to be enabled to use bootloader"
#endif

#if APP_USE_RTSP

#define FLASH_PAGE_MASK   0xFC00     // Gets 512-instruction aligned offset (1024)
#define FLASH_PAGE        0x0400     // Flash page size
#define FLASH_OFFSET_MASK 0x03FF
#define FLASH_ROW_MASK    0x007F
#define FLASH_ROW         0x0080

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



#if APP_USE_BOOTLOADER
// Non-secure (integrated) versions, required to update bootloader itself
#define SECURE(f) f
#define SCALL(f) f
#define NSCALL(f) f
#include "flash_secure.c"
#undef SECURE
#undef SCALL
#undef NSCALL
#endif

// Secure versions
#define SECURE(f) SECURE_DECL secure_##f
#define SCALL(f) secure_##f
#if APP_USE_BOOTLOADER
#define NSCALL(f) sub_##f
#include "bootloader_sub.h"
#else
#define NSCALL(f) f
#endif
#include "flash_secure.c"
#undef SECURE
#undef NSCALL


void flash_init() {
    if(reset_is_cold()) {
        flashOps = _flashop_list_create();
    }
}

void flash_set(unsigned char page, unsigned int offset, uint16_t value) {
    _FlashOp *operation = gc_malloc(sizeof(_FlashOp));
    operation->pageNum = page;
    operation->offset  = offset;
    operation->value   = value;
    list_push_back(flashOps, operation);
}

int flash_store_erase() {
    return SCALL(flash_erase_page)(FLASH_GETPAGE(pageStore),
            FLASH_GETOFFSET(pageStore));
}

int flash_store_writerow(unsigned int rowNum, _Instruction *row) {
    return SCALL(flash_writerow)(FLASH_GETPAGE(pageStore),
                FLASH_GETOFFSET(pageStore) + (rowNum<<7), row);
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
    
    _Instruction *progRow = _flash_tmp_instructionHold;

    offset &= FLASH_PAGE_MASK;

    // Prepare pageStor
    flash_store_erase();

    // Read page
    for(i = 0; i < 8; ++i) {
        SCALL(flash_readrow)(page, offset+(i>>7), progRow);
        flash_store_writerow(i, progRow);
    }
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
            if(SCALL(flash_writepage)(group)) {
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
