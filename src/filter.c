#include "system.h"
#include "filter.h"
#include "median.h"

#define CHANNEL_COUNT 7

// Input filtering
static _PERSISTENT long unsigned meanArr[CHANNEL_COUNT];
static _PERSISTENT Mediator *medians[CHANNEL_COUNT];
static _PERSISTENT long int rawValue[CHANNEL_COUNT];

typedef void(*FilterInputFunction_t)(unsigned int, uint8_t);
typedef unsigned int(*FilterOutputFunction_t)(uint8_t);

static _PERSISTENT FilterInputFunction_t fFilterInput;
static _PERSISTENT FilterOutputFunction_t fFilterOutput;

#define FILTER_COUNT 1024
#define FILTER_COUNT_2 10
#define MEDIAN_FILTER_COUNT 15

/* Moving median */

static void moving_median_gc() {
    int i;
    for (i = 0; i < CHANNEL_COUNT; ++i) {
        MediatorFree(medians[i]);
        medians[i] = 0;
    }
}

static void moving_median_init() {
    int i;
    
    if (reset_is_cold()) {
        for (i = 0; i < CHANNEL_COUNT; ++i) {
            medians[i] = 0;
        }
        garbage_collect_reg(moving_median_gc);
    }
}

static void moving_median_input(unsigned int value, uint8_t channel_num) {
    if (!medians[channel_num]) {
        medians[channel_num] = MediatorNew(MEDIAN_FILTER_COUNT);
    }
    MediatorInsert(medians[channel_num], value);
}

static unsigned int moving_median_output(uint8_t channel_num) {
    if (medians[channel_num]) {
        return MediatorMedian(medians[channel_num]);
    } else {
        return 0;
    }
}

/* Moving mean */

static void moving_mean_init() {
    int i;
    
    if (reset_is_cold()) {
        for (i = 0; i < CHANNEL_COUNT; ++i) {
            meanArr[i] = 0;
        }
    }
}

static void moving_mean_input (unsigned int value, uint8_t channel_num) {
    meanArr[channel_num] += (-(meanArr[channel_num]>>FILTER_COUNT_2) + value);
}

static unsigned int moving_mean_output(uint8_t channel_num) {
    return (meanArr[channel_num]>>FILTER_COUNT_2);
}

/* Raw filter */

static void raw_filter_init() {
    int i;
    
    if (reset_is_cold()) {
        for (i = 0; i < CHANNEL_COUNT; ++i) {
            rawValue[i] = 0;
        }
    }
}

static void raw_filter_input(unsigned int value, uint8_t channel_num) {
    rawValue[channel_num] = value;
}

static unsigned int raw_filter_output(uint8_t channel_num) {
    return rawValue[channel_num];
}

void filter_set_type(FilterType type) {
    switch(type) {
        case FilterTypeMovingMean:
            fFilterInput = moving_mean_input;
            fFilterOutput = moving_mean_output;
            break;
        case FilterTypeMovingMedian:
            fFilterInput = moving_median_input;
            fFilterOutput = moving_median_output;
            break;
        case FilterTypeNone:
        default:
            fFilterInput = raw_filter_input;
            fFilterOutput = raw_filter_output;
            break;
    }
}

void filter_put(unsigned int value, uint8_t channel_num) {
    fFilterInput(value, channel_num);
}

unsigned int filter_get(uint8_t channel_num) {
    return fFilterOutput(channel_num);
}

void filter_init() {
    raw_filter_init();
    moving_median_init();
    moving_mean_init();
    
    if (reset_is_cold()) {
        filter_set_type(FilterTypeNone);
    }
}
