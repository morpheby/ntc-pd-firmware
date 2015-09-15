

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

_PERSISTENT static Filter *adcInputFilter;
static float P1Sum = 0;
static float P2Sum = 0;
static float A0sum = 0;
static float A1sum = 0;
static float A2sum = 0;
static uint16_t counter = 0;
long int last_time;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeNone, 10);
    }
    
    last_time = timing_get_time_msecs();
}

MAIN_DECL_LOOP_FN() {
    
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
    
    // Extract ADC values and push to modbus
    MB.A0 = filter_get(adcInputFilter, 0);
    MB.A1 = filter_get(adcInputFilter, 1);
    MB.A2 = filter_get(adcInputFilter, 2);
        
    MB.ADC0 = (MB.A0 - MB.OFS_ADC0) * MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1) * MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2) * MB.K2;
    
    A0sum += MB.ADC0*MB.ADC0;
    A1sum += MB.ADC1*MB.ADC1;
    A2sum += MB.ADC2*MB.ADC2;
    
    P1Sum+=MB.ADC0*MB.ADC1;
    P2Sum+=MB.ADC0*MB.ADC2;
    
    counter++;    
    
    long int time = timing_get_time_msecs();
    long int dt = time - last_time;
    
    if(dt >= 1000) {
        MB.P1 = P1Sum / counter;
        MB.P2 = P2Sum / counter;
        MB.M0_RMS = sqrt(A0sum / counter);
        MB.M1_RMS = sqrt(A1sum / counter);
        MB.M2_RMS = sqrt(A2sum / counter);
        A0sum = 0;
        A1sum = 0;
        A2sum = 0;
        P1Sum = 0;
        P2Sum = 0;
        counter = 0;
        last_time = time;
    }
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
