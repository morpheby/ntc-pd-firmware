/* 
 * File:   filter.h
 * Author: evgeny
 *
 * Created on 7 Июль 2015 г., 15:30
 */

#ifndef FILTER_H
#define	FILTER_H

#include <stdint.h>

typedef unsigned int (*FilterFunction_t)(unsigned int, uint8_t);

// Filters ADC input data
unsigned int filter (unsigned int value, uint8_t channel_num);

typedef enum {
    AVG_VALUE_FILTER
} FilterType;

void setFilterType(FilterType type);

unsigned int doFilter (unsigned int value, uint8_t channel_num);


#endif	/* FILTER_H */

