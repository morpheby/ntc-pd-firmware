
#include "bootloader_sub.h"

#if APP_USE_BOOTLOADER


#define HIGHEST_PRIORITY 7
#define MAGIC            0xA5

//_FLASH_STORE _FLASH_ACCESS uint16_t __boot_from_main = 1;

uint8_t SECURE_DECL sub_cpu_ipl_set(int ipl) {
    uint8_t iss     = DISICNT,
            prevIPL;

    asm volatile ("disi #0x3FFF");
    
    prevIPL = SRbits.IPL;
    SRbits.IPL = ipl;
    
    __builtin_write_DISICNT(iss);

    return prevIPL;
}

SYSHANDLE SECURE_DECL sub_high_priority_enter() {
    SYSHANDLE i;
    uint8_t prevIPL = sub_cpu_ipl_set(HIGHEST_PRIORITY);
    i = ((MAGIC ^ prevIPL) << 8) | prevIPL;
    return i;
}

void SECURE_DECL sub_high_priority_exit(SYSHANDLE hp) {
    uint16_t prevIPL = hp;
    // Check handle integrity
    if( (uint8_t)prevIPL != (uint8_t) (MAGIC ^ (prevIPL >> 8)) ||
        sub_cpu_ipl_set(prevIPL) != HIGHEST_PRIORITY) {
        asm volatile("reset");
    }
}

#endif
