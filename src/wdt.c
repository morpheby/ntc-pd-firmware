
#include "wdt.h"

void wdt_init() {
#ifdef DEBUG
    // WDT intervenes debugging
    RCONbits.SWDTEN = 0;
#else
    RCONbits.SWDTEN = 1;
#endif
}

void wdt_clr() {
    ClrWdt();
}
