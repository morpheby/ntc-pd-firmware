

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
static float P1Sum = 0;
static float P2Sum = 0;
static float P3Sum = 0;
static float A0Sum = 0;
static float A1Sum = 0;
static float A2Sum = 0;
static float A0SquareSum = 0;
static float A1SquareSum = 0;
static float A2SquareSum = 0;
static uint16_t counter = 0;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeNone, 10);
    }
    discrete_set_output(MB.D_Out_Init);
}

MAIN_DECL_LOOP_FN() {
#if USE_DS1820_SENSORS
    if(MB.Control0 == 1) {
        DS1820_initROM();
        MB.Control0 = 0;
    }
    DS1820_update();
#else
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
#endif
    
    // Extract ADC values and push to modbus
    MB.A0 = filter_get(adcInputFilter, 0);
    MB.A1 = filter_get(adcInputFilter, 1);
    MB.A2 = filter_get(adcInputFilter, 2);
    MB.A3 = filter_get(adcInputFilter, 3);
    MB.A4 = filter_get(adcInputFilter, 4);
    MB.A5 = filter_get(adcInputFilter, 5);
    MB.A6 = filter_get(adcInputFilter, 6);
        
    MB.ADC0 = (MB.A0 - MB.OFS_ADC0) * MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1) * MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2) * MB.K2;
    MB.ADC3 = (MB.A3 - MB.OFS_ADC3) * MB.K3;
    MB.ADC4 = (MB.A4 - MB.OFS_ADC4) * MB.K4;
    MB.ADC5 = (MB.A5 - MB.OFS_ADC5) * MB.K5;
    MB.ADC6 = (MB.A6 - MB.OFS_ADC6) * MB.K6;
    
    A0SquareSum += MB.ADC0*MB.ADC0;
    A1SquareSum += MB.ADC1*MB.ADC1;
    A2SquareSum += MB.ADC2*MB.ADC2;
    
    A0Sum += MB.ADC0;
    A1Sum += MB.ADC1;
    A2Sum += MB.ADC2;
    
    P1Sum+=MB.ADC0*MB.ADC1;
    P2Sum+=MB.ADC0*MB.ADC2;
    P3Sum+=MB.ADC1*MB.ADC2;
    
    counter++;    
        
    if(counter >= MB.N) {
        MB.P1 = P1Sum / counter * MB.P1_coef;
        MB.P2 = P2Sum / counter * MB.P2_coef;
        MB.P3 = P3Sum / counter * MB.P3_coef;
        
        MB.M0_AVG = (A0Sum / counter);
        MB.M1_AVG = (A1Sum / counter);
        MB.M2_AVG = (A2Sum / counter);
        
        int M0_sign = 1;
        int M1_sign = 1;
        int M2_sign = 1;
        
        if(fabs(MB.M0_AVG) >= MB.M0_RMS_sign_threshold && MB.M0_AVG < 0) {
            M0_sign = -1;
        }
        
        if(fabs(MB.M1_AVG) >= MB.M1_RMS_sign_threshold && MB.M1_AVG < 0) {
            M1_sign = -1;
        }
        
        if(fabs(MB.M2_AVG) >= MB.M2_RMS_sign_threshold && MB.M2_AVG < 0) {
            M2_sign = -1;
        }
        
        MB.M0_RMS = sqrt(A0SquareSum / counter)*M0_sign;
        MB.M1_RMS = sqrt(A1SquareSum / counter)*M1_sign;
        MB.M2_RMS = sqrt(A2SquareSum / counter)*M2_sign;
                
        MB.S1 = MB.M0_RMS * MB.M1_RMS;
        MB.S2 = MB.M0_RMS * MB.M2_RMS;
        MB.S3 = MB.M1_RMS * MB.M2_RMS;
                        
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
                                 
        A0SquareSum = 0;
        A1SquareSum = 0;
        A2SquareSum = 0;
        
        A0Sum = 0;
        A1Sum = 0;
        A2Sum = 0;
        
        P1Sum = 0;
        P2Sum = 0;
        P3Sum = 0;
        
        counter = 0;
    }
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
