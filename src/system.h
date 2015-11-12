/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

#ifndef SYSTEM_H
#define	SYSTEM_H

/* Device header file */
#if defined(__dsPIC33E__)
	#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
	#include <p33Fxxxx.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "resource.h"

/* Microcontroller MIPs (FCY) */
#define CRYSTAL_FREQ        7370000L
#define LPRC_CRYSTAL_FREQ     32000L
extern uint32_t sysFreq;
#define SYS_FREQ        (sysFreq)
#define FCY             (SYS_FREQ/2)

#include <libpic30.h>
#define delay_us(d ) \
  { __delay32( (unsigned long) (((float) d)*(FCY)/1e6f)); }


#define _ISR_NOPSV      __attribute__((interrupt,no_auto_psv))

#define __RP_MAP(num) RP##num##R
#define __RPOR_MAP(num) (RPOR##num##bits)
#define RPOR_MAP(num, num2) __RPOR_MAP(num2).__RP_MAP(num)

#define __PIN_CTL4(ctl, ctl2, type, num) ctl##type##bits.ctl2##type##num
#define __PIN_CTL3(ctl, ctl2, type, num) __PIN_CTL4(ctl,ctl2,type,num)
#define __PIN_CTL2(ctl, ctl2, type, num) __PIN_CTL3(ctl,ctl2,type,num)
#define __PIN_CTL(ctl, type, num) __PIN_CTL2(ctl,ctl,type,num)
#define __PIN_CTL_EXT(ctl, ctl2, type, num) __PIN_CTL2(ctl,ctl2,type,num)
#define TRIS_BIT(type, num) __PIN_CTL(TRIS, type, num)
#define PIN_LATCH(type, num) __PIN_CTL(LAT, type, num)
#define PIN_PORT(type, num) __PIN_CTL_EXT(PORT, R, type, num)
#define ODC_BIT(type, num) __PIN_CTL(ODC, type, num)

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

// XXX validate declarations

typedef void *SYSHANDLE;

void close_handle(SYSHANDLE handle);

void system_init();

/* Reset CPU */
void system_reset() __attribute__((noreturn));

/* Executed in case of serious system failure */
void system_fail(const char *reason) __attribute__((noreturn));

/* Disable interrupts for 16384 instructions */
void interrupts_disable();

/* Force interrupt enable */
void interrupts_enable();

/* Store current disable-interrupt counter state */
int interrupts_store_state();

/*
 * Restore saved state (may be used with zero arg to
 * enable interrupts. However interrupts_enable() is
 * preferred)
 */
void interrupts_restore_state(int state);

uint8_t cpu_ipl_get();

/*
 * Set processor Interrupt Priority Level.
 * Returns previous IPL
 */
uint8_t cpu_ipl_set(int ipl);

/* 
 * SYSHANDLE high_priority_enter()
 * Starts high-priority operation.
 * Returns SYSHANDLE to previous state
 */
SYSHANDLE high_priority_enter();

/*
 * void high_priority_exit(SYSHANDLE hp)
 * Exits high priority section.
 * hp is state stored by high_priority_start.
 * SYSHANDLE is closed internally, no need to call close_handle
 */
void high_priority_exit(SYSHANDLE hp);

/* Sets bit in OSCCONL register, preserving all other bits */
void osccon_set_bit(_Bool value, uint8_t bitNum);

/* Enables or disables pin remap (releases or sets IOLOCK) */
void pin_remap_enable(_Bool enable);

typedef enum tagRESETTYPE {
    _Reset_POR,
    _Reset_BOR,
    _Reset_MCLR,
    _Reset_Soft,
    _Reset_WatchDog,
    _Reset_ConfMismatch,
    _Reset_TrapConflict,
    _Reset_ILL,     // IOPUWR illegal opcode, uninitialized W reg or
                    // security reset
} _ResetType;

_ResetType reset_type_get();
const char *last_failure_get();

/* Returns 1 if reset was cold and all data was cleared */
_Bool reset_is_cold();

/* Locks system for the given number of cycles */
void system_lock(uint16_t cycles);

void garbage_collect_do();

void garbage_collect_reg(void (*gcFuncPtr)());

void *gc_malloct(size_t size); // Crashes system in case no more memory available
void *gc_malloc(size_t size); // Returns 0 if no more memory available
void *gc_realloc(void *mem, size_t size); // Returns 0 if no more memory available
void gc_free(void *mem);


void led_set(_Bool on);
void led_on();
void led_off();
_Bool led_state();
void led_change();
void led_flash(uint32_t t);

#endif	/* SYSTEM_H */
