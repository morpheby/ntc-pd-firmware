#include "timing.h"
#include "ipl-config.h"
#include "menu-base.h"
#include "menu.h"
#include "messaging.h"

#define MAIN_TIMER_PERIOD_DFLT 65535

_PERSISTENT _Bool tmMsrMode;

void timing_init() {
    if(reset_is_cold())
        tmMsrMode = 0;

    T1CONbits.TON = 0;
    T1CONbits.TCS = 0;
    T1CONbits.TGATE = 0;
    T1CONbits.TSYNC = 0; // Enable Synchronization
    T1CONbits.TCKPS = 0b00; // 00 - 1:1
                            // 01 - 1:8
                            // 10 - 1:64
                            // 11 - 1:256
    TMR1 = 0x00;
    PR1 = MAIN_TIMER_PERIOD_DFLT;

    IPC0bits.T1IP = IPL_TIMER1;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;

    T3CONbits.TON = 0;
    T2CONbits.TON = 0;
    T2CONbits.T32 = 1;
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.TCKPS = 0b00;

    timer32_reset();
    timer32_set_period((uint32_t)-1L);

    IPC2bits.T3IP = IPL_TIMER32;
    IFS0bits.T3IF = 0;
}

void timing_set_period(uint16_t period) {
    if(period)
        PR1 = period;
    else
        PR1 = MAIN_TIMER_PERIOD_DFLT;
}

void timer32_start() {
    timer32_reset();
    IEC0bits.T3IE = 1;
    T2CONbits.TON = 1;
}

void timer32_reset() {
    TMR3 = 0x00;
    TMR2 = 0x00;
}

void timer32_stop() {
    IEC0bits.T3IE = 0;
    T2CONbits.TON = 1;
}

void timer32_set_period(uint32_t period) {
    PR3 = period >> 16;
    PR2 = period;
}

uint32_t timer32_get() {
    uint32_t val = TMR2;
    val |= ((uint32_t)TMR3HLD) << 16;
    return val;
}

void timer32_set(uint32_t val) {
    TMR3HLD = val >> 16;
    TMR2 = val;
}

void _ISR_NOPSV _T1Interrupt(void) {
    disp_draw();
    menu_worker();
    messages_check();
    IFS0bits.T1IF = 0;
}

void _ISR_NOPSV _T3Interrupt(void) {
    IFS0bits.T3IF = 0;
}
