#include "oscillators.h"

#if HAS_EXTERNAL_OSCILLATOR
#define PRIMARY_CRYSTAL_FREQ 8000000L
#endif

void osc_switch(_OSCILATOR osc) {
    SYSHANDLE prevState = high_priority_enter();

    // Initiate Clock Switch
    __builtin_write_OSCCONH((unsigned char) osc);
    // Switch (first bit in OSCCONL)
    osccon_set_bit(1, 0);
    
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != (unsigned char) osc);

    high_priority_exit(prevState);
}

/* Fast internal oscillator */
void osc_frc() {
    osc_switch(oscillatorFRC);
}

/* Low-Power oscillator */
void osc_lprc() {
    osc_switch(oscillatorLPRC);
}

/* Fast internal oscillator with PLL */
void osc_frc_pll() {
    osc_switch(oscillatorFRCPLL);

    // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1) {}
}

#if HAS_EXTERNAL_OSCILLATOR
void osc_primary() {
    osc_switch(oscillatorPrimary);
}

void osc_primary_pll() {
    osc_switch(oscillatorPrimaryPLL);

    // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1) {}
}
#endif

uint32_t osc_get_freq() {
    switch (OSCCONbits.COSC) {
    case oscillatorFRC:
        return CRYSTAL_FREQ;
    case oscillatorFRCPLL:
        return CRYSTAL_FREQ *
            (PLLFBD + 2) /
            ( (CLKDIVbits.PLLPOST + 2) * (CLKDIVbits.PLLPRE + 2) );
    case oscillatorLPRC:
        return LPRC_CRYSTAL_FREQ;
#if HAS_EXTERNAL_OSCILLATOR
    case oscillatorPrimary:
        return PRIMARY_CRYSTAL_FREQ;
    case oscillatorPrimaryPLL:
        return PRIMARY_CRYSTAL_FREQ *
            (PLLFBD + 2) /
            ( (CLKDIVbits.PLLPOST + 2) * (CLKDIVbits.PLLPRE + 2) );
#endif
    default:
        return 0;
    }
}

/*
 * _OSCILATOR osc_select(_OSCILATOR oscType)
 * Switches to the oscType oscilator and
 * updates frequency.
 * Returns previous oscillator
 */
_OSCILATOR osc_select(_OSCILATOR oscType) {
    uint16_t prevOsc = OSCCONbits.COSC;
    switch(oscType) {
    case oscillatorFRC:
        osc_frc();
        break;
    case oscillatorFRCPLL:
        osc_frc_pll();
        break;
    case oscillatorLPRC:
        osc_lprc();
        break;
#if HAS_EXTERNAL_OSCILLATOR
    case oscillatorPrimary:
        osc_primary();
        break;
    case oscillatorPrimaryPLL:
        osc_primary_pll();
        break;
#endif
    }
    
    // Update SYS_FREQ
    sysFreq = osc_get_freq();

    return prevOsc;
}

/* Initializes Phase-locked loop to operate on 40(39) MIPS */
void osc_pll_init() {
    // see dsPIC33F doc section 7.7
    PLLFBD = 41; // M = 43
    CLKDIVbits.PLLPOST=0; // N2 = 2
    CLKDIVbits.PLLPRE=0; // N1 = 2
}

void osc_init() {
    osc_pll_init();
    
    // Update SYS_FREQ
    sysFreq = osc_get_freq();
}

