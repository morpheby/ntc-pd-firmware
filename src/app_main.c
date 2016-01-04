

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

void app_init() {
    adcInputFilter = filter_create(7, FilterTypeMovingMean, 3);
}


void perform_data_operations() {
    MB.ADC_M0 = filter_get(adcInputFilter, 0);
    MB.ADC_M1 = filter_get(adcInputFilter, 1);
    MB.ADC_M2 = filter_get(adcInputFilter, 2);
    MB.ADC_A0 = filter_get(adcInputFilter, 3);
    MB.ADC_A1 = filter_get(adcInputFilter, 4);
    MB.ADC_A2 = filter_get(adcInputFilter, 5);
    MB.ADC_A3 = filter_get(adcInputFilter, 6);
    
    MB.M0_value = (MB.ADC_M0 - MB.M0_OFFSET)*MB.M0_Coef;
    MB.M1_value = (MB.ADC_M1 - MB.M1_OFFSET)*MB.M1_Coef;
    MB.M2_value = (MB.ADC_M2 - MB.M2_OFFSET)*MB.M2_Coef;
    MB.A0_value = (MB.ADC_A0 - MB.A0_OFFSET)*MB.A0_Coef;
    MB.A1_value = (MB.ADC_A1 - MB.A1_OFFSET)*MB.A1_Coef;
    MB.A2_value = (MB.ADC_A2 - MB.A2_OFFSET)*MB.A2_Coef;
    MB.A3_value = (MB.ADC_A3 - MB.A3_OFFSET)*MB.A3_Coef;
        
    
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
