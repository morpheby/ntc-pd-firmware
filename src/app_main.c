

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
_PERSISTENT static Filter *squaredFilter;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeMovingMean, 10);
        squaredFilter = filter_create(6, FilterTypeMovingMean, 8);
    }
    initQEI();
    initPWM();
}

MAIN_DECL_LOOP_FN() {
    
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
    
    // Extract ADC values and push to modbus
    MB.A0 = filter_get(adcInputFilter, 0) - MB.OFS_ADC0;
    MB.A1 = filter_get(adcInputFilter, 1) - MB.OFS_ADC1;
    MB.A2 = filter_get(adcInputFilter, 2) - MB.OFS_ADC2;
    MB.A3 = filter_get(adcInputFilter, 3) - MB.OFS_ADC3;
    MB.A4 = filter_get(adcInputFilter, 4) - MB.OFS_ADC4;
    MB.A5 = filter_get(adcInputFilter, 5) - MB.OFS_ADC5;
    MB.A6 = filter_get(adcInputFilter, 6) - MB.OFS_ADC6;
    
    // Process ADC values
    MB.ADC0 = MB.A0 * MB.K0;
    MB.ADC1 = MB.A1 * MB.K1;
    MB.ADC2 = MB.A2 * MB.K2;
    MB.ADC3 = MB.A3 * MB.K3;
    MB.ADC4 = MB.A4 * MB.K4;
    MB.ADC5 = MB.A5 * MB.K5;
    MB.ADC6 = MB.A6 * MB.K6;
    
    // Calculate all possible P
    filter_put(squaredFilter, ((long)MB.A0)*MB.A1, 3);
    filter_put(squaredFilter, ((long)MB.A0)*MB.A2, 4);
    filter_put(squaredFilter, ((long)MB.A1)*MB.A2, 5);
    MB.P1 = filter_get(squaredFilter, 3) * MB.K0 * MB.K1;
    MB.P2 = filter_get(squaredFilter, 4) * MB.K0 * MB.K2;
    MB.P3 = filter_get(squaredFilter, 5) * MB.K1 * MB.K2;
    
    // Calculate RMS values
    filter_put(squaredFilter, ((long)MB.A0)*MB.A0, 0);
    filter_put(squaredFilter, ((long)MB.A1)*MB.A1, 1);
    filter_put(squaredFilter, ((long)MB.A2)*MB.A2, 2);
    MB.M0_RMS = sqrt(filter_get(squaredFilter, 0)) * MB.K0;
    MB.M1_RMS = sqrt(filter_get(squaredFilter, 1)) * MB.K1;
    MB.M2_RMS = sqrt(filter_get(squaredFilter, 2)) * MB.K2;
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
