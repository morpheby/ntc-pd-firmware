

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
#include <string.h>
#include <stdlib.h>

#include "quadrature_encoder.h"
#include  "menu-base.h"
#include "PWM.h"
#include "ind_profiles.h"
#include "filter.h"

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

_PERSISTENT static Filter *adcInputFilter;
_PERSISTENT static float adcSquareExpMovingMean[7];
_PERSISTENT static float diImpPeriodeExpMovingMean[4];
_PERSISTENT static long diImpTime[4];

#define FREQ_ALPHA 0.2
#define FREQ_BETA 0.8
#define DI_IMP_TIMEOUT_MS 3000

void app_init() {
    adcInputFilter = filter_create(7, FilterTypeMovingMean, 4);
    uint8_t i;
    for(i = 0; i < 7; ++i) {
        adcSquareExpMovingMean[i] = 0;
    }
    for(i = 0; i < 4; ++i) {
        diImpPeriodeExpMovingMean[i] = 0;
        diImpTime[i] = 0;
    }
    initQEI();
    disp_set_off(1,1);
    disp_set_off(2,1);
    disp_set_off(3,1);
    MB.TimerValue = 0;
}

void perform_data_operations() {
    MB.D_In = discrete_get_input();
    discrete_set_output(MB.D_Out);
    
    if(PIN_PORT(DI2_PIN_TYPE, DI2_PIN_NUM)){
        if(!time_mesuring_started()){
            start_time_mesuring();            
        }
        float time = get_mesured_time();
        if(time < 1000.0f) {
            MB.TimerValue = time;             
        }        
    } else {
        stop_time_mesuring();
    }
    if(PIN_PORT(DI3_PIN_TYPE, DI3_PIN_NUM)) {
        reset_time_mesuring();
        MB.TimerValue = 0;
    }
    
    MB.QEI_W_rad_s = QEI_getSpeedRadS();
    MB.QEI_W_rot_s = QEI_getSpeedRotS();
        
    float alpha = 1.0f/MB.N;
    float beta = 1.0f-alpha;
    
    uint8_t i;
    static int *adcSourceValuesPtr = &MB.ADC_M0;
    static int *adcOffsetsPtr = &MB.M0_OFFSET;
    static float *adcCoefsPtr = &MB.M0_Coef;
    static float *adcValuesPtr = &MB.M0_value; 
    static float *avgValuesPtr = &MB.M0_AVG; 
    static float *rmsValuesPtr = &MB.M0_RMS;  
    static float *rmsSignThreshPtr = &MB.M0_RMS_sign_threshold;
    
    for(i = 0; i < 7; ++i){
        adcSourceValuesPtr[i] = filter_get(adcInputFilter, i);
        adcValuesPtr[i] = (adcSourceValuesPtr[i]-adcOffsetsPtr[i])*adcCoefsPtr[i];
        avgValuesPtr[i] = adcValuesPtr[i] * alpha + beta*avgValuesPtr[i];
        adcSquareExpMovingMean[i] = adcValuesPtr[i] * adcValuesPtr[i] * alpha + beta*adcSquareExpMovingMean[i];
        float sign = 1.0;
        if(fabs(avgValuesPtr[i]) > fabs(rmsSignThreshPtr[i]) && avgValuesPtr[i] < 0){
            sign = -1.0;
        }
        rmsValuesPtr[i] = sqrt(adcSquareExpMovingMean[i])*sign;
    }
  
    MB.P1 = MB.M0_value*MB.M1_value*MB.P1_coef*alpha + beta*MB.P1;
    MB.P2 = MB.M0_value*MB.M2_value*MB.P2_coef*alpha + beta*MB.P2;
    MB.P3 = MB.M1_value*MB.M2_value*MB.P3_coef*alpha + beta*MB.P3;
    
    MB.S1 = MB.M0_RMS*MB.M1_RMS;
    MB.S2 = MB.M0_RMS*MB.M1_RMS;
    MB.S3 = MB.M1_RMS*MB.M2_RMS;
    
    if(fabs(MB.P1) >= MB.S1)
    {
        MB.Q1 = 0;
        MB.cos_f1 = 1;
    }
    else
    {
        MB.Q1 = sqrt(MB.S1 * MB.S1 - MB.P1 * MB.P1); //значения реактивной мощности ?????
        MB.cos_f1 = MB.P1 / MB.S1;
    }
        
    if(fabs(MB.P2) >= MB.S2)
    {
        MB.Q2 = 0;
        MB.cos_f2 = 1;
    }
    else
    {
        MB.Q2 = sqrt(MB.S2 * MB.S2 - MB.P2 * MB.P2); //значения реактивной мощности ?????
        MB.cos_f2 = MB.P2 / MB.S2;
    }
        
    if(fabs(MB.P3) >= MB.S3)
    {
        MB.Q3 = 0;
        MB.cos_f3 = 1;
    }
    else
    {
        MB.Q3 = sqrt(MB.S3 * MB.S3 - MB.P3 * MB.P3); //значения реактивной мощности ?????
        MB.cos_f3 = MB.P3 / MB.S3;
    }
    
    long time = timing_get_time_msecs();
    float *impFreq = &MB.DI0_ImpFreq;
    float *impFreqCoef = &MB.DI0_ImpFreqCoef;

    for(i = 0; i < 4; ++i) {
        if(time - diImpTime[0] < DI_IMP_TIMEOUT_MS) {
            impFreq[i] = 1.0f/diImpPeriodeExpMovingMean[i]*impFreqCoef[i];
        } else {
            impFreq[i] = 0;
        }
    }
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}

#if COUNT_DI0_IMP_FREQUENCY
//DI0 impulse counter
CNI_DECL_PROC_FN(29, on) {
    static bool prev = 0;
    if(on != prev) 
    {
        MB.DI0_ImpCount++;
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
#endif

#if COUNT_DI1_IMP_FREQUENCY
//DI1 impulse counter
CNI_DECL_PROC_FN(30, on) {
    static bool prev = 0;
    if(on != prev) 
    {   
        MB.DI1_ImpCount++;
        prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[1]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[1] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[1];
            }
            diImpTime[0] = time;
        }
    }
}
#endif

#if COUNT_DI2_IMP_FREQUENCY
//DI2 impulse counter
CNI_DECL_PROC_FN(10, on) {
    static bool prev = 0;
    if(on != prev) 
    {   
        MB.DI2_ImpCount++;
        prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[2]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[2] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[2];
            }
            diImpTime[0] = time;
        }
    }
}
#endif


#if COUNT_DI3_IMP_FREQUENCY
//DI3 impulse counter
CNI_DECL_PROC_FN(9, on) {
    static bool prev = 0;
    if(on != prev) 
    {   
        MB.DI3_ImpCount++;
        prev = on;
        if(on) {
            long time = timing_get_time_msecs();
            long dt = (time - diImpTime[3]);
            if(dt < DI_IMP_TIMEOUT_MS) {
                diImpPeriodeExpMovingMean[3] = dt*0.001 * FREQ_ALPHA + FREQ_BETA * diImpPeriodeExpMovingMean[3];
            }
            diImpTime[0] = time;
        }
    }
}
#endif