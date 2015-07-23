#include "quadrature_encoder.h"
#include "system.h"
#include "board-config.h"

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
   // DFLTCONbits.QEOUT = 1;     // Digital filters output enabled for QEn pins
    //DFLTCONbits.QECK = 5;      // 1:64 clock divide for digital filter for QEn
    POSCNT = 1;                // Reset position counter
    QEICONbits.QEIM = 7;       // X4 mode with position counter reset by MATCH (_QEIInterrupt is called on reset)
    MAXCNT = 0xFFFF;           // Max position counter value
    return;
}


void __attribute__((interrupt,no_auto_psv)) _QEIInterrupt() {

     IFS3bits.QEIIF = 0;
}