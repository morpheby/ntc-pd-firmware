#include "system.h"
#include "median.h"

#define FILTER_C
#include "filter.h"


typedef void(*FilterInputFunction_t)(Filter *, unsigned int, uint8_t);
typedef unsigned int(*FilterOutputFunction_t)(Filter *, uint8_t);
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
#define MedianFilterSize(count) (sizeof(MedianFilter)+sizeof(Mediator)*count)

typedef struct MeanFilter_t {
    FILTER_T;
    long unsigned arr[0];
} MeanFilter;
#define MeanFilterSize(count) (sizeof(MeanFilter)+sizeof(long unsigned)*count)

typedef struct RawFilter_t {
    FILTER_T;
    unsigned int arr[0];
} RawFilter;
#define RawFilterSize(count) (sizeof(RawFilter)+sizeof(unsigned int)*count)


/* Moving median */

static void moving_median_init(MedianFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->medians[i] = MediatorNew(filter->param);
    }
}

static void moving_median_input(MedianFilter *filter, unsigned int value,
        uint8_t channel_num) {
    MediatorInsert(filter->medians[channel_num], value);
}

static unsigned int moving_median_output(MedianFilter *filter,
        uint8_t channel_num) {
    return MediatorMedian(filter->medians[channel_num]);
}

/* Moving mean */

static void moving_mean_init(MeanFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->arr[i] = 0;
    }
}

static void moving_mean_input (MeanFilter *filter, unsigned int value,
        uint8_t channel_num) {
    filter->arr[channel_num] +=
            (-(filter->arr[channel_num]>>filter->param) + value);
}

static unsigned int moving_mean_output(MeanFilter *filter,
        uint8_t channel_num) {
    return filter->arr[channel_num]>>filter->param;
}

/* Raw filter */

static void raw_filter_init(RawFilter *filter) {
    int i;
    
    for (i = 0; i < filter->channelCount; ++i) {
        filter->arr[i] = 0;
    }
}

static void raw_filter_input(RawFilter *filter, unsigned int value,
        uint8_t channel_num) {
    filter->arr[channel_num] = value;
}

static unsigned int raw_filter_output(RawFilter *filter, uint8_t channel_num) {
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
        case FilterTypeMovingMean:
            allocSize = MeanFilterSize(channelCount);
            fInput = (FilterInputFunction_t)moving_mean_input;
            fOutput = (FilterOutputFunction_t)moving_mean_output;
            fInit = (FilterInitFunction_t)moving_mean_init;
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

void filter_put(Filter *filter, unsigned int value, uint8_t channel_num) {
    filter->fFilterInput(filter, value, channel_num);
}

unsigned int filter_get(Filter *filter, uint8_t channel_num) {
    return filter->fFilterOutput(filter, channel_num);
}

void filter_init() {
}
