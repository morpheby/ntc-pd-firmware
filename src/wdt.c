#include "wdt.h"

void wdt_init() {
    // Enabled in _FWDT
//    RCONbits.SWDTEN = 1;
}

void wdt_clr() {
    ClrWdt();
}
