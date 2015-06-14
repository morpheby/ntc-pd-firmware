/* 
 * File:   oscillators.h
 * Author: Morphe
 *
 * Created on 30 ???? 2012 ?., 7:12
 */

#ifndef OSCILLATORS_H
#define	OSCILLATORS_H

#include "system.h"

#define HAS_EXTERNAL_OSCILLATOR 0

typedef enum tagOSCILLATOR {
    oscillatorFRC        = 0b000,
    oscillatorFRCPLL     = 0b001,
    oscillatorLPRC       = 0b101,
#if HAS_EXTERNAL_OSCILLATOR
    oscillatorPrimary    = 0b010,
    oscillatorPrimaryPLL = 0b011,
#endif
} _OSCILATOR;

typedef enum tagOscType {
#if HAS_EXTERNAL_OSCILLATOR
    oscNormal   = oscillatorPrimary,
    oscBoosted  = oscillatorPrimaryPLL,
#else
    oscNormal = oscillatorFRC,
    oscBoosted  = oscillatorFRCPLL,
#endif
    oscLowPower = oscillatorLPRC
} _OscType;

/*
 * _OSCILATOR osc_select(_OSCILATOR oscType)
 * Switches to the oscType oscilator and
 * updates frequency.
 * Returns previous oscillator
 */
_OSCILATOR osc_select(_OSCILATOR oscType);

/* Initializes oscillators (i.e. PLL configuration) */
void osc_init();

#endif	/* OSCILLATORS_H */

