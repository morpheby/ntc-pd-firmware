
#include "flash_store.h"
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

typedef struct tagINSTRUCTION {
    uint8_t  highByte;
    uint16_t lowWord;
} _Instruction;


// Temporary store for page
uint16_t pageStore[512] __attribute__((space(prog),aligned(2048)));
_PERSISTENT _Instruction _flash_tmp_instructionHold[64];

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

void flash_write_direct(unsigned char page, unsigned int offset, uint16_t value)
{
    SYSHANDLE hp = high_priority_enter();
    
    uint16_t dataOffset = offset;
    offset &= FLASH_PAGE_MASK;
     
    flash_readpage(page, offset);
    
    // While we will be rewriting page, some parts of program may become invalid
    // Ensure we are not erasing this exact function
    if(FLASH_GETPAGE(flash_write_direct) == page &&
       (FLASH_GETOFFSET(flash_write_direct) ^ dataOffset) < FLASH_PAGE){
        return;
    }
    
    flash_erase_page(page, offset);
    
    _Instruction *row = _flash_tmp_instructionHold;
    
    int i;
    for(i = 0; i < 8; ++i) {
        // Load row
        flash_store_readrow(i, row);
        
        if((dataOffset & FLASH_OFFSET_MASK) / 128 == i) {
                    row[(dataOffset & FLASH_ROW_MASK) / 2].lowWord = value;
        }

        flash_writerow(page, offset+i*128, row);
    }
    
    high_priority_exit(hp);
}

#else

void flash_write_direct(unsigned char page, unsigned int offset, uint16_t value){}

#endif

