/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"          /* variables/params used by system.c             */

#include <stdlib.h>
#include <reset.h>

#include "mem_pool.h"

/*
 * Note on SYSHANDLE
 *  Currently SYSHANDLE simply represents memory location, allocated with
 *  malloc. Anything can be stored in it.
 */

// Highest available user-level CPU priority in the system
#define HIGHEST_PRIORITY 7
#define MAGIC            0xA5

#define USE_PERSISTENT_HEAP 1
#define HAS_LED 0

#if HAS_LED
#define LED_PIN_TPYE
#define LED_PIN_NUM
#endif

#if USE_PERSISTENT_HEAP
#define GC_MALLOC mp_alloc
#define GC_REALLOC mp_realloc
#define GC_FREE mp_free
#else
#define GC_MALLOC malloc
#define GC_REALLOC realloc
#define GC_FREE free
#endif

/* Oscillation frequency definition */
_PERSISTENT uint32_t sysFreq;

_PERSISTENT const char *lastFailReason;

const char *lastFail = 0;

_ResetType lastReset = 0;

typedef struct tagGCFUNCLIST {
    void (*gcFPtr) ();
    void *prev;
} _GCFuncList;

_PERSISTENT _GCFuncList *gcFuncs;

void __attribute__((noreturn)) system_reset() {
    asm volatile("reset");
    while(1);
}

void interrupts_disable() {
    asm volatile ("disi #0x3FFF");
}

int interrupts_store_state() {
    return DISICNT;
}

void interrupts_restore_state(int state) {
    __builtin_write_DISICNT(state);
}

void interrupts_enable() {
    interrupts_restore_state(0);
}

uint8_t cpu_ipl_get() {
    return SRbits.IPL;
}

uint8_t cpu_ipl_set(int ipl) {
    uint8_t iss     = interrupts_store_state(),
            prevIPL;

    interrupts_disable();
    
    prevIPL = SRbits.IPL;
    SRbits.IPL = ipl;
    
    interrupts_restore_state(iss);

    return prevIPL;
}

SYSHANDLE high_priority_enter() {
    uint16_t *i = gc_malloc(sizeof(uint16_t));
    uint8_t prevIPL = cpu_ipl_set(HIGHEST_PRIORITY);
    *i = ((MAGIC ^ prevIPL) << 8) | prevIPL; // secure handle
    return i;
}

void high_priority_exit(SYSHANDLE hp) {
    uint16_t prevIPL = * (uint16_t*)hp;
    // Check handle integrity
    if( (uint8_t)prevIPL != (uint8_t) (MAGIC ^ (prevIPL >> 8)) ||
        cpu_ipl_set(prevIPL) != HIGHEST_PRIORITY) {
        // Quite a serious error has happened, may be either critical
        // error in HP use order or buffer overrun
        close_handle(hp);  // Clean resources first
        system_fail(_StrHPError);
    }
    close_handle(hp);
}

void close_handle(SYSHANDLE handle) {
    gc_free(handle);
}

void osccon_set_bit(_Bool value, uint8_t bitNum) {
    uint8_t val = OSCCONL;
    if(bitNum>7)
        return;
    if(value)
        val |= value << bitNum;
    else
        val &= ~ (value << bitNum);
    __builtin_write_OSCCONL(val);
}

void pin_remap_enable(_Bool enable) {
    // clear OSCCONbits.IOLOCK<6>
    osccon_set_bit(!enable, 6);
}

/* A one-time function for reseet_init*/
_ResetType _reset_type_get() {
    if(isPOR())
        return _Reset_POR;
    else if(isBOR())
        return _Reset_BOR;
    else if(isMCLR())
        return _Reset_MCLR;
    else if(isSWR())
        return _Reset_Soft;
    else if(isWDTTO())
        return _Reset_WatchDog;
    else if(isCM())
        return _Reset_ConfMismatch;
    else if(isTRAPR())
        return _Reset_TrapConflict;
    else if(isIOPUWR())
        return _Reset_ILL;
    else return -1; // Never should happen
}

void reset_init() {
    lastReset = _reset_type_get();
}

_ResetType reset_type_get() {
    return lastReset;
}

_Bool reset_is_cold() {
    return reset_type_get() == _Reset_POR ||
           reset_type_get() == _Reset_BOR;
}

void system_lock(uint16_t cycles) {
    SYSHANDLE hPriority = high_priority_enter();
    while(--cycles);
    high_priority_exit(hPriority);
}

void garbage_collect_do() {
    _GCFuncList *gc = gcFuncs;

    while(gc) {
        gc->gcFPtr();
        gc = gc->prev;
    }
}

void garbage_collect_reg(void (*gcFuncPtr)()) {
    _GCFuncList *newElem = gc_malloc(sizeof(_GCFuncList));

    newElem->gcFPtr = gcFuncPtr;
    newElem->prev   = gcFuncs;

    gcFuncs = newElem;
}

void garbage_collect_init() {
    gcFuncs = 0;
}

void *gc_malloc(size_t size) {
    void *mem = GC_MALLOC(size);
    if(!mem) {
        garbage_collect_do();
        mem = GC_MALLOC(size);
        if(!mem)
            system_fail(_StrNoMemory);
    }
    return mem;
}
void *gc_realloc(void *mem, size_t size) {
    void *newMem;
    if(!mem)
        newMem = GC_MALLOC(size);
    else
        newMem = GC_REALLOC(mem, size);
    if(!newMem) {
        garbage_collect_do();
        newMem = GC_REALLOC(mem, size);
        if(!newMem)
            system_fail(_StrNoMemory);
    }
    return newMem;
}

void gc_free(void *mem) {
    GC_FREE(mem);
}

void fail_init() {
    if(reset_is_cold())
        lastFailReason = 0;
    lastFail = lastFailReason;
    lastFailReason = 0;
}

void led_init() {
    TRISBbits.TRISB4 = 0; // set RB4 to output
}

void system_init() {
    led_init();
    reset_init();
    fail_init();
    if(reset_is_cold())
        sysFreq = CRYSTAL_FREQ;
    garbage_collect_init();
}

void __attribute__((noreturn)) system_fail(const char *reason) {
    /* Executed on serious system failure */
    lastFailReason = reason;

    // XXX For debugging hang system
//    while(1) {
//        led_flash(7000000L);
//    }
    // XXX For production reset system
    system_reset();
}

const char *last_failure_get() {
    if(lastFail)
        return lastFail;
    else
        // ??? consider removing unnecessary messages
        return _StrResetDescr[reset_type_get()];
}

void led_set(_Bool on) {
#if HAS_LED
    PIN_LATCH(LED_PIN_TYPE, LED_PIN_NUM) = on;
#endif
}

void led_on() {
    led_set(1);
}

void led_off() {
    led_set(0);
}

_Bool led_state() {
#if HAS_LED
    return PIN_LATCH(LED_PIN_TYPE, LED_PIN_NUM);
#else
    return 0;
#endif
}

void led_change() {
    led_set(!led_state());
}

void led_flash(uint32_t t) {
    t /= 2; // flash-on flash-off
    t /= 12; // instructions in counter
    uint32_t counter = t;
    if(counter == 0)
        return;
    led_change();
    while(--counter);
    led_change();
    counter = t;
    while(--counter);
}
