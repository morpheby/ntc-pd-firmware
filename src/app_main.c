

/*
 * Mandatory inculdes
 */
#include "system.h"
#include "app_main.h"
#include "app_connector.h"
#include "board-config.h"

/*
 * App-specific includes
 */
#include "timing.h"
#include "D_I_O.h"
#include "modbus_registers.h"
#include "float.h"
#include <stdint.h>
#include <math.h>

#include "quadrature_encoder.h"
#include  "menu-base.h"
#include "PWM.h"
#include "ind_profiles.h"
#include "filter.h"
#include "DS1820.h"

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

_PERSISTENT static Filter *adcInputFilter;
_PERSISTENT static Filter *squaredFilter;

static unsigned int DI0_counter = 0;
static unsigned int DI1_counter = 0;
static long int last_time;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeMovingMean, 10);
    }
    DS1820_initROM();
    last_time = timing_get_time_msecs();
}

MAIN_DECL_LOOP_FN() {
    // Extract ADC values and push to modbus
    MB.A0 = filter_get(adcInputFilter, 0);
    MB.A1 = filter_get(adcInputFilter, 1);
    MB.A2 = filter_get(adcInputFilter, 2);
    MB.A3 = filter_get(adcInputFilter, 3);
    
    // Process ADC values
    MB.ADC0 = (MB.A0- MB.OFS_ADC0) * MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1) * MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2) * MB.K2;
    MB.ADC3 = (MB.A3 - MB.OFS_ADC3) * MB.K3;
   
    DS1820_update();
    MB.DS1820_TEMP_1 = DS1820_temperature(0);
    MB.DS1820_TEMP_2 = DS1820_temperature(1);
    MB.DS1820_TEMP_3 = DS1820_temperature(2);
    MB.DS1820_TEMP_4 = DS1820_temperature(3);
    MB.DS1820_TEMP_5 = DS1820_temperature(4);
    MB.DS1820_TEMP_6 = DS1820_temperature(5);
    MB.DS1820_TEMP_7 = DS1820_temperature(6);
    MB.DS1820_TEMP_8 = DS1820_temperature(7);
    MB.DS1820_TEMP_9 = DS1820_temperature(8);
    MB.DS1820_TEMP_10 = DS1820_temperature(9);
    MB.DS1820_TEMP_11 = DS1820_temperature(10);
    
    long int time = timing_get_time_msecs();
    long int dt = time - last_time;
    
    if(dt >= 1000) {
        MB.DI0_ImpFrequency = 1000.0f*(float)DI0_counter / (float)dt;
        MB.DI1_ImpFrequency = 1000.0f*(float)DI1_counter / (float)dt;
        DI0_counter = 0;
        DI1_counter = 0;
        last_time = time;
    }
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}

//DI0 impulse counter
CNI_DECL_PROC_FN(29, on) {
    if(!on) 
    {
        DI0_counter++;        
    }
}

//DI1 impulse counter
CNI_DECL_PROC_FN(30, on) {
    if(!on) 
    {
        DI1_counter++;        
    }
}
