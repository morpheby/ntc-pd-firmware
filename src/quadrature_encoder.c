#include "quadrature_encoder.h"
#include "system.h"
#include "board-config.h"
#include "timing.h"
#include "modbus_registers.h"

static float speedRadS;
static float speedRotS;
static uint32_t timeMSecs;

#define PI (3.141592653589793)

void initQEI()
{
    //Pins configuration. Data Sheet pages 120, 131
    pin_remap_enable(1);
    RPINR14bits.QEA1R = QEIA;
    RPINR14bits.QEB1R = QEIB;
    pin_remap_enable(0);

    QEICONbits.QEIM = 0;       // Disable QEI Module
    QEICONbits.CNTERR = 0;     // Clear any count errors
    QEICONbits.QEISIDL = 0;    // Continue operation during sleep
    QEICONbits.SWPAB = 0;      // QEA and QEB not swapped
    QEICONbits.PCDOUT = 0;     // Normal I/O pin operation
    QEICONbits.POSRES = 0;     // Index pulse resets position counter
    DFLTCONbits.CEID = 1;      // Count error interrupts disabled
    DFLTCONbits.QEOUT = 1;     // Digital filters output enabled for QEn pins
    DFLTCONbits.QECK = 5;      // 1:64 clock divide for digital filter for QEn
    POSCNT = 1;                // Reset position counter
    QEICONbits.QEIM = 7;       // X4 mode with position counter reset by MATCH (_QEIInterrupt is called on reset)
    MAXCNT = MB.disk_imp_count;           // Max position counter value. Defined by MB reg and equals number of impulses per full rotation

    timeMSecs = timing_get_time_msecs(); // store current system time

    return;
}

void __attribute__((interrupt,no_auto_psv)) _QEIInterrupt()
{
    uint32_t time = timing_get_time_msecs(); // get current system time
    uint32_t dT = time - timeMSecs; // get time between full rotations
    timeMSecs = time; // store current system time;

    int16_t sign = 1;
    if(!QEICONbits.UPDN) //direction is negative
    {
        sign = -1;
    }
    speedRotS = sign*1000.0f/(float)dT; // calculate rotation speed in rotations per second;
    speedRadS = PI*2*speedRotS; // calculate rotation speed in radians per second;

    IFS3bits.QEIIF = 0;
}

float QEI_getSpeedRadS()
{
    return speedRadS;
}

float QEI_getSpeedRotS()
{
    return speedRotS;
}