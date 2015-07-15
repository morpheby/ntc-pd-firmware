
#include "wdt.h"

void wdt_init() {
    RCONbits.SWDTEN = 1;
}

void wdt_clr() {
    ClrWdt();
}
