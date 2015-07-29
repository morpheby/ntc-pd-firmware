/* 
 * File:   filter.h
 * Author: evgeny
 *
 * Created on 7 Июль 2015 г., 15:30
 */

#ifndef FILTER_H
#define	FILTER_H

#include "system.h"
#include "app_connector.h"

typedef enum {
    FilterTypeMovingMean,
    FilterTypeMovingMedian,
    FilterTypeNone,
} FilterType;

// Initialize filter module
void filter_init();

// Set filter type
void filter_set_type(FilterType type);

// Stores value for the filter
void filter_put(unsigned int value, uint8_t channel_num);

// Gets current value from the filter
unsigned int filter_get(uint8_t channel_num);

#endif	/* FILTER_H */

