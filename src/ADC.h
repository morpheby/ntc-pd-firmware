/* 
 * File:   ADC.h
 * Author: 
 *
 * Created on 2 April 2015
 */
#ifndef ADC_H
#define ADC_H

#include "filter.h"
#include "modbus_registers.h"

#define MAX_ADC_CHANNEL_COUNT 7

void ADC_Init(_Bool ad_12b);

#endif