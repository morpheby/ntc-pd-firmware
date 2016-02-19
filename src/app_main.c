

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

_PERSISTENT static float diImpPeriodeExpMovingMean[3];
_PERSISTENT static long diImpTime[3];

#define FREQ_ALPHA 0.6
#define FREQ_BETA 0.4
#define DI_IMP_TIMEOUT_MS 11000

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
    
    for(i = 0; i < 3; ++i) {
        diImpPeriodeExpMovingMean[i] = 0;
        diImpTime[i] = 0;
    }
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
        last_time = time;
    }
    
    if(time - diImpTime[0] < DI_IMP_TIMEOUT_MS && diImpPeriodeExpMovingMean[0] > 0) {
        MB.DI0_ImpFrequency = 1.0f/diImpPeriodeExpMovingMean[0]*MB.DI0_ImpCoef;
    } else {
        MB.DI0_ImpFrequency = 0;
        diImpPeriodeExpMovingMean[0] = 0;
    }
    
    if(time - diImpTime[1] < DI_IMP_TIMEOUT_MS && diImpPeriodeExpMovingMean[1] > 0) {
        MB.DI1_ImpFrequency = 1.0f/diImpPeriodeExpMovingMean[1]*MB.DI1_ImpCoef;
    } else {
        MB.DI1_ImpFrequency = 0;
        diImpPeriodeExpMovingMean[1] = 0;
    }
    
    if(time - diImpTime[2] < DI_IMP_TIMEOUT_MS && diImpPeriodeExpMovingMean[2] > 0) {
        MB.DI2_ImpFrequency = 1.0f/diImpPeriodeExpMovingMean[2]*MB.DI2_ImpCoef;
    } else {
        MB.DI2_ImpFrequency = 0;
        diImpPeriodeExpMovingMean[2] = 0;
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
        prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[0]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[0] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[0];
            }
            diImpTime[0] = time;
        }
    }
}

//DI1 impulse counter
CNI_DECL_PROC_FN(30, on) {
    static bool prev = 0;
    if(on != prev) 
    {   
        prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[1]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[1] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[1];
            }
            diImpTime[1] = time;
        }
    }
}
//DI2 impulse counter
CNI_DECL_PROC_FN(10, on) {
    static bool prev = 0;
    if(on != prev) 
    {   prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[2]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[2] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[2];
            }
            diImpTime[2] = time;
        }
    }
}