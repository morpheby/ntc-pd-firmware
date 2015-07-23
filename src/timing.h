/* 
 * File:   timing.h
 * Author: Morphe
 *
 * Created on 25 ?????? 2012 ?., 12:05
 */

#ifndef TIMING_H
#define	TIMING_H

#include "system.h"

typedef struct _tagSYTEMTIME {
    uint32_t lowDWord, highDWord;
} _SystemTime;

typedef struct _tagTIME {
    uint32_t secs, nsecs;
} _time_t;

void timing_init();

// Timer32
void timer32_start();
void timer32_stop();
void timer32_reset();

uint32_t timer32_get();

// Set interrupt and value clear period in Timer32
void timer32_set_period_raw(uint32_t period);
uint32_t timer32_get_period_raw();

uint32_t timer32_get_period(); // Period in nanosecs

_SystemTime timing_get_systime();
uint32_t timing_get_time_low();
uint32_t timing_get_time_msecs();


// Main timer
uint16_t timer_get();

uint16_t timer_get_period_raw();
void timer_set_period_raw(uint16_t period);

uint32_t timer_get_period(); // Period in nanosecs

_time_t timing_get_time();

/*
 * -1: lsv  < rsv
 *  0: lsv == rsv
 *  1: lsv  > rsv
 */
int time_compare(_time_t lsv, _time_t rsv);

void time_add(_time_t *dst, _time_t add);

// NOTE: dst should be greater than sub. Otherwise result is undefined
void time_sub(_time_t *dst, _time_t sub);

#endif	/* TIMING_H */

