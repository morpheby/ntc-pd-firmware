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

typedef unsigned int (*FilterFunction_t)(unsigned int, uint8_t);

typedef enum {
    _movingMeanFilterType,
    _movingMedianFilterType,
} FilterType;

void setFilterType(FilterType type);

unsigned int doFilter (unsigned int value, uint8_t channel_num);


#endif	/* FILTER_H */

