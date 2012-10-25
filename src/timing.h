/* 
 * File:   timing.h
 * Author: Morphe
 *
 * Created on 25 ?????? 2012 ?., 12:05
 */

#ifndef TIMING_H
#define	TIMING_H

#include "system.h"

void timing_init();
void timing_measure_mode_on();
void timer32_start();
void timer32_stop();
void timer32_reset();
void timer32_set_period(uint32_t period);
uint32_t timer32_get();
void timer32_set(uint32_t val);

#endif	/* TIMING_H */

