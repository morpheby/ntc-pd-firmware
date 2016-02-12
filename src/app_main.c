

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
static unsigned int DI2_counter = 0;
static long int last_time;

static bool heaterOn = 0;
static bool coolerOn = 0;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeMovingMean, 10);
    } 
    last_time = timing_get_time_msecs();
    int i = 0;
    uint16_t *ROM = &MB.TermoId_0_bytes_0_1;
    uint16_t *ROM_Storage = &MB.Env_TermoId_bytes_0_1;
    for(i=0; i<12; ++i){
        ROM[i] = ROM_Storage[i];
    }
    MB.TermoCount = 3;
}

MAIN_DECL_LOOP_FN() {
    // Extract ADC values and push to modbus
    MB.A0 = filter_get(adcInputFilter, 0);
    MB.A1 = filter_get(adcInputFilter, 1);
    MB.A2 = filter_get(adcInputFilter, 2);
    MB.A3 = filter_get(adcInputFilter, 3);
    MB.A4 = filter_get(adcInputFilter, 4);
    MB.A5 = filter_get(adcInputFilter, 5);
    MB.A6 = filter_get(adcInputFilter, 6);
    
    // Process ADC values
    MB.ADC0 = (MB.A0- MB.OFS_ADC0) * MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1) * MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2) * MB.K2;
    MB.ADC3 = (MB.A3 - MB.OFS_ADC3) * MB.K3;
    MB.ADC4 = (MB.A4 - MB.OFS_ADC4) * MB.K4;
    MB.ADC5 = (MB.A5 - MB.OFS_ADC5) * MB.K5;
    MB.ADC6 = (MB.A6 - MB.OFS_ADC6) * MB.K6;
   
    if(MB.Control0 == 1) {
        DS1820_initROM();
        MB.Control0 = 0;   
    }
    
    long int time = timing_get_time_msecs();
    long int dt = time - last_time;
    
    if(dt >= MB.TERMO_Update_Interval) {
        DS1820_update();
        MB.DI0_ImpFrequency = 500.0f*(float)DI0_counter / (float)dt*MB.DI0_ImpCoef;
        MB.DI1_ImpFrequency = 500.0f*(float)DI1_counter / (float)dt*MB.DI1_ImpCoef;
        MB.DI2_ImpFrequency = 500.0f*(float)DI2_counter / (float)dt*MB.DI2_ImpCoef;
        DI0_counter = 0;
        DI1_counter = 0;
        DI2_counter = 0;
        last_time = time;
    }
    if(MB.DS1820_TEMP_2 >= MB.T_hot_min + MB.delta_T_hist){
        heaterOn = 0;        
    } else if(MB.DS1820_TEMP_2 <= MB.T_hot_min){
        heaterOn = 1;        
    }
    
    if(MB.DS1820_TEMP_3 >= MB.delta_T_hist + MB.DS1820_TEMP_1){
        coolerOn = 1;      
    } else if(MB.DS1820_TEMP_3 <= MB.DS1820_TEMP_1){
        coolerOn = 0;        
    }
    
    PIN_LATCH(VT2_PIN_TYPE, VT2_PIN_NUM) = heaterOn;
    PIN_LATCH(VT3_PIN_TYPE, VT3_PIN_NUM) = coolerOn;    
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}

//DI0 impulse counter
CNI_DECL_PROC_FN(29, on) {
    static bool prev = 0;
    if(on != prev) 
    {
        DI0_counter++;    
        prev = on;
    }
}

//DI1 impulse counter
CNI_DECL_PROC_FN(30, on) {
    static bool prev = 0;
    if(on != prev) 
    {
        DI1_counter++;  
        prev = on;
    }
}
//DI2 impulse counter
CNI_DECL_PROC_FN(10, on) {
    static bool prev = 0;
    if(on != prev) 
    {
        DI2_counter++;  
        prev = on;
    }
}