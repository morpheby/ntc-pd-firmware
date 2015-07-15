
void SECURE(_flash_start)() {
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

_Bool SECURE(_flash_ok)() {
    return !NVMCONbits.WRERR;
}

void SECURE(_flash_preconf_row)() {
    NVMCONbits.WREN = 1;        // Enable write
    NVMCONbits.ERASE = 0;       // Don't erase
    NVMCONbits.NVMOP = 0b0001;  // Single row write
}

void SECURE(_flash_preconf_erase)() {
    NVMCONbits.WREN = 1;        // Enable write
    NVMCONbits.ERASE = 1;       // Erase
    NVMCONbits.NVMOP = 0b0010;  // Single page erase
}

/* Returns 0 if successful */
int SECURE(_flash_program_row)() {
    SCALL(_flash_preconf_row)();   // Configure flashing operation
    SCALL(_flash_start)();         // Flash
    return !SCALL(_flash_ok)();
}

/* Returns 0 if successful */
int SECURE(_flash_erase_page)() {
    SCALL(_flash_preconf_erase)();   // Configure flashing operation
    SCALL(_flash_start)();           // Flash
    return !SCALL(_flash_ok)();
}

int SECURE(flash_erase_page)(unsigned char page, unsigned int offset) {
    TBLPAG = page;
    __builtin_tblwtl(offset, 0);
    return SCALL(_flash_erase_page)();
}

int SECURE(flash_writerow)(unsigned char page, unsigned int offset,
        _Instruction *row) {
    int i;
    TBLPAG = page;
    
    // Write 64 instructions == 1 row
    for(i = 0; i < 128; i+=2) {
        __builtin_tblwtl(offset + i, row[i/2].lowWord);
        __builtin_tblwth(offset + i, row[i/2].highByte);
    }
    // Flash row
    return SCALL(_flash_program_row)();
}

void SECURE(flash_readrow)(unsigned char page, unsigned int offset,
        _Instruction *row) {
    int i;
    TBLPAG = page;
    // Read 64 instructions == 1 row
    for(i = 0; i < 128; i+=2) {
        row[i/2].lowWord  = __builtin_tblrdl(offset + i);
        row[i/2].highByte = __builtin_tblrdh(offset + i);
    }
}

void SECURE(flash_store_readrow)(unsigned int rowNum, _Instruction *row) {
    SCALL(flash_readrow)(FLASH_GETPAGE(pageStore),
                FLASH_GETOFFSET(pageStore) + rowNum*128, row);
}

/* Return 0 on success and 1 if flashing error has occured */
int SECURE(flash_writepage)(_ListHandle pageGroup) {
    int i, k = 0, opsSize, result = 0;
    _ListIterator j;
    _FlashOp op = *(_FlashOp*)NSCALL(list_front)(pageGroup),
            *ops;
    _Instruction *row = _flash_tmp_instructionHold;
    
    unsigned int offset;
    SYSHANDLE hp;


    // While we will be rewriting page, some parts of program may become invalid
    // Ensure we are not erasing this exact function
    if(FLASH_GETPAGE(SCALL(flash_writepage)) == op.pageNum &&
       (FLASH_GETOFFSET(SCALL(flash_writepage)) ^ op.offset) < FLASH_PAGE)
        return 2;

    // Preload list of changes
    j = NSCALL(list_begin)(pageGroup);
    do {
        ++k;
    } while(NSCALL(list_iterate_fwd)(j));
    NSCALL(list_iterator_free)(j);

    ops = NSCALL(gc_malloc)(sizeof(_FlashOp)*k);

    k = 0;
    j = NSCALL(list_begin)(pageGroup);
    do {
        ops[k++] = *(_FlashOp*) NSCALL(list_iterator_value)(j);
    } while(NSCALL(list_iterate_fwd)(j));
    NSCALL(list_iterator_free)(j);

    opsSize = k;


    hp = NSCALL(high_priority_enter)();
        // Prepare page
        SCALL(flash_erase_page)(op.pageNum, op.offset);
        offset = op.offset & FLASH_PAGE_MASK;

        // Group by row
        for(i = 0; i < 8; ++i) {
            // Load row
            SCALL(flash_store_readrow)(i, row);

            // Apply all operations from that row
            for(k = 0; k < opsSize; ++k)
                if((ops[k].offset & FLASH_OFFSET_MASK) / 128 == i)
                    row[(ops[k].offset & FLASH_ROW_MASK) / 2].lowWord = ops[k].value;

            // Store row
            if(SCALL(flash_writerow)(op.pageNum, offset+i*128, row)) {
                result = 1;
                break;
            }
        }
    NSCALL(high_priority_exit)(hp);
    NSCALL(gc_free)(ops);

    return result;
}
