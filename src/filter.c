#include "system.h"
#include "filter.h"
#include "median.h"

// Input filtering
static _PERSISTENT long int arr[7]= {0, 0, 0, 0, 0, 0, 0};
static _PERSISTENT unsigned int init[7] = {0, 0, 0, 0, 0, 0, 0};
static _PERSISTENT Mediator *medians[7];

#define FILTER_COUNT 1024
#define FILTER_COUNT_2 10
#define MEDIAN_FILTER_COUNT 15

unsigned int moving_median(unsigned int value, uint8_t channel_num) {
    if (!medians[channel_num]) {
        medians[channel_num] = MediatorNew(MEDIAN_FILTER_COUNT);
    }
    MediatorInsert(medians[channel_num], value);
    return MediatorMedian(medians[channel_num]);
}

unsigned int moving_mean (unsigned int value, uint8_t channel_num) {
    if (init[channel_num] < FILTER_COUNT) {
        arr[channel_num] += value;
        ++init[channel_num];
        return 0;
    } else {
        arr[channel_num] += (-(arr[channel_num]>>FILTER_COUNT_2) + value);
        return (arr[channel_num]>>FILTER_COUNT_2);
    }
}

static _PERSISTENT FilterFunction_t filterFunction = 0;

void setFilterType(FilterType type) {
    switch(type) {
        case _movingMeanFilterType: filterFunction = moving_mean; break;
        default: filterFunction = moving_mean; break;
    }
}

unsigned int doFilter (unsigned int value, uint8_t channel_num)
{
    if(!filterFunction) {
        return value;
    }
    return (filterFunction)(value, channel_num);
}