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
    /*
     * Moving mean filter
     * param = log2 of count of used values in buffer
     */
    FilterTypeMovingMean,
    /*
     * Moving median filter
     * param = count of used values in buffer
     */
    FilterTypeMovingMedian,
    /*
     * Raw value filter
     * Doesn't filter, provides last value raw
     * param is unused
     */
    FilterTypeNone,
} FilterType;

#ifndef FILTER_C
typedef void Filter;
#else
typedef struct Filter_t Filter;
#endif

// Initialize filter module
void filter_init();

// Create new filter with selected type and count of channels and
// filtering characteristic (implementation defined)
Filter *filter_create(uint8_t channelCount, FilterType type, uint16_t param);

// Stores value for the filter
void filter_put(Filter *filter, unsigned int value, uint8_t channel_num);

// Gets current value from the filter
unsigned int filter_get(Filter *filter, uint8_t channel_num);

#endif	/* FILTER_H */

