#include "filter.h"

// Input filtering
static long int arr[7]= {0, 0, 0, 0, 0, 0, 0};
static unsigned int init[7] = {0, 0, 0, 0, 0, 0, 0};

#define FILTER_COUNT 1000

unsigned int filter (unsigned int  value, uint8_t channel_num) {
    if (init[channel_num] < FILTER_COUNT) {
        arr[channel_num] += value;
        ++init[channel_num];
        return 0;
    } else {
        arr[channel_num] += (-arr[channel_num]/FILTER_COUNT + value);
        return arr[channel_num]/FILTER_COUNT;
    }
}

static FilterFunction_t filterFunction = 0;

void setFilterType(FilterType type) {
    switch(type) {
        case AVG_VALUE_FILTER: filterFunction = filter; break;
        default: filterFunction = filter; break;
    }
}

unsigned int doFilter (unsigned int value, uint8_t channel_num)
{
    if(!filterFunction) {
        return value;
    }
    return (filterFunction)(value, channel_num);
}