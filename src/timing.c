#include "timing.h"
#include "ipl-config.h"
#include "menu-base.h"
#include "menu.h"
#include "util.h"

#define MAIN_TIMER_PERIOD_DFLT 65535

// System time in nanoseconds
static _SystemTime _systemTime = {0, 0};

static _Bool _useHighResTimer;

void timing_init() {
    T1CONbits.TON = 0;      // Turn off Timer 1
    T1CONbits.TCS = 0;      // 0 - Internal clock, 1 - External clock (TxCK pin)
    T1CONbits.TGATE = 0;    // Don't use Gated Timer mode
    T1CONbits.TSYNC = 0;    // 1 - Synchronize external clock input, 0 - Don't synchronize
                            // When TCS=0 - unused
    T1CONbits.TCKPS = 0b01; // Prescaler:
                            // 00 - 1:1
                            // 01 - 1:8
                            // 10 - 1:64
                            // 11 - 1:256
    
    TMR1 = 0x00;            // Reset Timer register
    
    // Initialize timer period
    timer_set_period_raw(0);

    // Setup Timer1 interrupt handling
    IPC0bits.T1IP = IPL_TIMER1;  // IPL
    IFS0bits.T1IF = 0;           // Clear interrupt flag
    IEC0bits.T1IE = 1;           // Enable interrupt
    
    // Start Timer1
    T1CONbits.TON = 1;


    // Configure Timer2+Timer3 in 32-bit mode
    T3CONbits.TON = 0;      // Turn off both timers
    T2CONbits.TON = 0;
    T2CONbits.T32 = 1;      // Enable 32-bit operation mode
    T2CONbits.TCS = 0;      // Internal clock source
    T2CONbits.TGATE = 0;    // Normal operation, not gated
    T2CONbits.TCKPS = 0b00; // 1:1 Prescale

    // Reset counters
    timer32_reset();
    
    // Set maximum possible period
    timer32_set_period_raw((uint32_t)-1L);

    // Setup Timer2 interrupts
    IPC2bits.T3IP = IPL_TIMER32;  // IPL
    IFS0bits.T3IF = 0;            // Clear interrupt flag
    IEC0bits.T2IE = 1;            // Set interrupts enable flag
    
    // Don't turn on timer32 unless requested explicitly
}

void timer32_start() {
    timer32_reset();
    
    // When we use timer32, prefer it over main timer for system time value
    _useHighResTimer = 1;
    
    IEC0bits.T3IE = 1;
    T2CONbits.TON = 1;
}

void timer32_reset() {
    TMR3 = 0x00;
    TMR2 = 0x00;
}

void timer32_stop() {
    IEC0bits.T3IE = 0;
    T2CONbits.TON = 0;
    
    _useHighResTimer = 1;
}

uint32_t timer32_get() {
    uint32_t val = TMR2;
    val |= ((uint32_t)TMR3HLD) << 16;
    return val;
}

uint16_t timer_get() {
    return TMR1;
}

uint16_t timer_get_period_raw() {
    return PR1;
}

void timer_set_period_raw(uint16_t period) {
    if(period)
        PR1 = period;
    else
        PR1 = MAIN_TIMER_PERIOD_DFLT;
}

uint32_t timer_tick_period_internal(uint8_t timer) {
    static uint32_t tmpFCY = 0;
    static uint8_t tmpTckps[2] = {0, 0};
    static uint32_t period[2];
    
    if (tmpFCY != FCY || tmpTckps[timer] != T1CONbits.TCKPS) {
        uint32_t freq;
        tmpFCY = FCY;
        tmpTckps[timer] = timer == 0 ? T1CONbits.TCKPS : T2CONbits.TCKPS;
        freq = tmpFCY >> (tmpTckps[timer] * 3);
        
        period[timer]  = (1000000000L/freq);
    }
    return period[timer];
}

uint32_t timer_get_period() {
    return timer_get_period_raw() * timer_tick_period_internal(0);
}

void timer32_set_period_raw(uint32_t period) {
    PR3 = period >> 16;
    PR2 = period;
}

uint32_t timer32_get_period_raw() {
    return ((uint32_t)PR2) | (((uint32_t)PR3)<<16);
}

uint32_t timer32_get_period() {
    return timer_get_period_raw() * timer_tick_period_internal(1);
    
}

uint32_t timing_time_add(uint32_t *low, uint32_t *high, uint32_t diff) {
    uint32_t t;
    
    t = *low;
    *low += diff;
    
    if (high && *low < t){
        ++*high;
    }
    
    return *low;
}

/* timerSource: 0 - Timer1 interrupt,
 *              1 - Timer2 interrupt,
 */
void timing_time_increment(int8_t timerSource) {
    uint32_t incrementValue;
    
    switch (timerSource) {
        case 0:
            incrementValue = timer_get_period();
            break;
        case 1:
            incrementValue = timer32_get_period();
            break;
    }
    timing_time_add(
            &(_systemTime.lowDWord),
            &(_systemTime.highDWord),
            incrementValue);
}

_SystemTime timing_get_systime() {
    _SystemTime tm = _systemTime;
    timing_time_add(
            &(tm.lowDWord),
            &(tm.highDWord),
            _useHighResTimer
                ? timer_tick_period_internal(1) * timer32_get()
                : timer_tick_period_internal(0) * timer_get()
            );
    return tm;
}

uint32_t timing_get_time_low() {
    return timing_get_systime().lowDWord;
}

uint32_t timing_get_time_msecs() {
    _SystemTime tm = timing_get_systime();
    return tm.lowDWord/1000000L + tm.highDWord * 4294L +
            (tm.highDWord * 967L)/1000L;
}

_time_t timing_get_time() {
    _SystemTime tm = timing_get_systime();
    _time_t time = {
        tm.lowDWord/1000000000L +
                tm.highDWord * 4 +
                (tm.highDWord * 295L)/1000L,
        tm.lowDWord % 1000000000L
    };
    return time;
}

int time_compare(_time_t lsv, _time_t rsv) {
    int32_t r = uint32_cmp(lsv.secs, rsv.secs);
    if (r) {
        return r;
    }
    r = lsv.nsecs - rsv.nsecs;
    if (r > 0) {
        return 1;
    } else if (r < 0) {
        return -1;
    }
    return 0;
}

void time_add(_time_t *dst, _time_t add) {
    dst->nsecs += add.nsecs;
    if (dst->nsecs > 1000000000L) {
        dst->nsecs -= 1000000000L;
        ++dst->secs;
    }
    dst->secs += add.secs;
}

void time_sub(_time_t *dst, _time_t sub) {
    dst->secs -= sub.secs;
    if (dst->nsecs < sub.nsecs) {
        dst->nsecs += 1000000000L;
        --dst->secs;
    }
    dst->nsecs -= sub.nsecs;
}

void _ISR_NOPSV _T1Interrupt(void) {
    // Increase system time value
    if (!_useHighResTimer) {
        timing_time_increment(0);
    }
    
    //ind_showValues();
    disp_draw();
    menu_worker();
    IFS0bits.T1IF = 0;
}

void _ISR_NOPSV _T3Interrupt(void) {
    // Timer 3 is used as a high-resolution timing module.
    // Prefer not to use this interrupt function
    
    // Increase system time value
    timing_time_increment(1);
    
    IFS0bits.T3IF = 0;
}
