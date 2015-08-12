#include "system.h"
#include "median.h"

#define FILTER_C
#include "filter.h"


typedef void(*FilterInputFunction_t)(Filter *, long, uint8_t);
typedef long(*FilterOutputFunction_t)(Filter *, uint8_t);
typedef void(*FilterInitFunction_t)(Filter *);

#define FILTER_T                            \
    uint8_t channelCount;                   \
    uint16_t param;                         \
    FilterInputFunction_t fFilterInput;     \
    FilterOutputFunction_t fFilterOutput

struct Filter_t {
    FILTER_T;
};

typedef struct MedianFilter_t {
    FILTER_T;
    Mediator *medians[0];
} MedianFilter;
#define MedianFilterSize(count) (sizeof(MedianFilter)+sizeof(Mediator*)*count)

typedef struct MeanFilter_t {
    FILTER_T;
    long arr[0];
} MeanFilter;
#define MeanFilterSize(count) (sizeof(MeanFilter)+sizeof(long)*count)

typedef struct RawFilter_t {
    FILTER_T;
    long arr[0];
} RawFilter;
#define RawFilterSize(count) (sizeof(RawFilter)+sizeof(long)*count)


/* Moving median */

void moving_median_init(MedianFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->medians[i] = MediatorNew(filter->param);
    }
}

void moving_median_input(MedianFilter *filter, long value,
        uint8_t channel_num) {
    MediatorInsert(filter->medians[channel_num], value);
}

long moving_median_output(MedianFilter *filter,
        uint8_t channel_num) {
    return MediatorMedian(filter->medians[channel_num]);
}

/* Moving mean */

void moving_mean_init(MeanFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->arr[i] = 0;
    }
}

void moving_mean_input (MeanFilter *filter, long value,
        uint8_t channel_num) {
    filter->arr[channel_num] +=
            (-(filter->arr[channel_num]>>filter->param) + value);
}

long moving_mean_output(MeanFilter *filter,
        uint8_t channel_num) {
    return filter->arr[channel_num]>>filter->param;
}

/* Raw filter */

void raw_filter_init(RawFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->arr[i] = 0;
    }
}

void raw_filter_input(RawFilter *filter, long value,
        uint8_t channel_num) {
    filter->arr[channel_num] = value;
}

long raw_filter_output(RawFilter *filter, uint8_t channel_num) {
    return filter->arr[channel_num];
}

Filter *filter_create(uint8_t channelCount, FilterType type, uint16_t param) {
    size_t allocSize;
    Filter *filter;
    FilterInputFunction_t fInput;
    FilterOutputFunction_t fOutput;
    FilterInitFunction_t fInit;
    
    switch(type) {
        case FilterTypeMovingMedian:
            allocSize = MedianFilterSize(channelCount);
            fInput = (FilterInputFunction_t)moving_median_input;
            fOutput = (FilterOutputFunction_t)moving_median_output;
            fInit = (FilterInitFunction_t)moving_median_init;
            break;
        case FilterTypeMovingMean:
            allocSize = MeanFilterSize(channelCount);
            fInput = (FilterInputFunction_t)moving_mean_input;
            fOutput = (FilterOutputFunction_t)moving_mean_output;
            fInit = (FilterInitFunction_t)moving_mean_init;
            break;
        case FilterTypeNone:
        default:
            allocSize = RawFilterSize(channelCount);
            fInput = (FilterInputFunction_t)raw_filter_input;
            fOutput = (FilterOutputFunction_t)raw_filter_output;
            fInit = (FilterInitFunction_t)raw_filter_init;
            break;
    }
    
    filter = gc_malloct(allocSize);
    filter->channelCount = channelCount;
    filter->param = param;
    filter->fFilterInput = fInput;
    filter->fFilterOutput = fOutput;
    fInit(filter);
    
    return filter;
}

void filter_put(Filter *filter, long value, uint8_t channel_num) {
    filter->fFilterInput(filter, value, channel_num);
}

long filter_get(Filter *filter, uint8_t channel_num) {
    return filter->fFilterOutput(filter, channel_num);
}

void filter_init() {
}
