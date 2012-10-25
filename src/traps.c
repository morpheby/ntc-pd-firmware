/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"
#include "oscillators.h"

/******************************************************************************/
/* Trap Function Prototypes                                                   */
/******************************************************************************/

/* <Other function prototypes for debugging trap code may be inserted here>   */

/* Use if INTCON2 ALTIVT=1 */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void);
void __attribute__((interrupt,no_auto_psv)) _AddressError(void);
void __attribute__((interrupt,no_auto_psv)) _StackError(void);
void __attribute__((interrupt,no_auto_psv)) _MathError(void);

/* Incompatible with dsPIC33FJ32MC204 */
//void __attribute__((interrupt,no_auto_psv)) _DMACError(void);

#if defined(__dsPIC33F__)

/* Use if INTCON2 ALTIVT=0 */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail(void);
void __attribute__((interrupt,no_auto_psv)) _AltAddressError(void);
void __attribute__((interrupt,no_auto_psv)) _AltStackError(void);
void __attribute__((interrupt,no_auto_psv)) _AltMathError(void);

/* Incompatible with dsPIC33FJ32MC204 */
//void __attribute__((interrupt,no_auto_psv)) _AltDMACError(void);

#endif

/* Default interrupt handler */
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt(void);

#if defined(__dsPIC33E__)

/* These are additional traps in the 33E family.  Refer to the PIC33E
migration guide.  There are no Alternate Vectors in the 33E family. */
void __attribute__((interrupt,no_auto_psv)) _HardTrapError(void);
void __attribute__((interrupt,no_auto_psv)) _SoftTrapError(void);

#endif

/******************************************************************************/
/* Trap Handling                                                              */
/*                                                                            */
/* These trap routines simply ensure that the device continuously loops       */
/* within each routine.  Users who actually experience one of these traps     */
/* can add code to handle the error.  Some basic examples for trap code,      */
/* including assembly routines that process trap sources, are available at    */
/* www.microchip.com/codeexamples                                             */
/******************************************************************************/

/* Primary (non-alternate) address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void)
{
    // Try to reinitialize oscillator
    OSCCONbits.CF = 0;
    osc_select(oscillatorPrimaryPLL);
    if(OSCCONbits.CF) // Failed to load the oscillator...
        system_fail(_StrOscFail);
    INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
}

void __attribute__((interrupt,no_auto_psv)) _AddressError(void)
{
        INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
        system_fail(_StrAddrError);
}
void __attribute__((interrupt,no_auto_psv)) _StackError(void)
{
        INTCON1bits.STKERR = 0;         /* Clear the trap flag */
        system_fail(_StrStackError);
}

void __attribute__((interrupt,no_auto_psv)) _MathError(void)
{
        INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
        system_fail(_StrMathError);
}

/* Incompatible with dsPIC33FJ32MC204 */
// __attribute__((interrupt,no_auto_psv)) _DMACError(void)
//{
//        INTCON1bits.DMACERR = 0;        /* Clear the trap flag */
//        system_fail(_StrDMAError);
//}

#if defined(__dsPIC33F__)

/* Alternate address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
        system_fail(_StrOscFail);
}

void __attribute__((interrupt,no_auto_psv)) _AltAddressError(void)
{
        INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
        system_fail(_StrAddrError);
}

void __attribute__((interrupt,no_auto_psv)) _AltStackError(void)
{
        INTCON1bits.STKERR = 0;         /* Clear the trap flag */
        system_fail(_StrStackError);
}

void __attribute__((interrupt,no_auto_psv)) _AltMathError(void)
{
        INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
        system_fail(_StrMathError);
}

/* Incompatible with dsPIC33FJ32MC204 */
//void __attribute__((interrupt,no_auto_psv)) _AltDMACError(void)
//{
//        INTCON1bits.DMACERR = 0;        /* Clear the trap flag */
//        system_fail(_StrDMAError);
//}


#endif

/******************************************************************************/
/* Default Interrupt Handler                                                  */
/*                                                                            */
/* This executes when an interrupt occurs for an interrupt source with an     */
/* improperly defined or undefined interrupt handling routine.                */
/******************************************************************************/
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt(void)
{
        system_fail(_StrUndefISR);
}

#if defined(__dsPIC33E__)

/* These traps are new to the dsPIC33E family.  Refer to the device Interrupt
chapter of the FRM to understand trap priority. */
void __attribute__((interrupt,no_auto_psv)) _HardTrapError(void)
{
}
void __attribute__((interrupt,no_auto_psv)) _SoftTrapError(void)
{
}

#endif
