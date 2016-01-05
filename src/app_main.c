

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
#include "DS1820.h"

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

_PERSISTENT static Filter *adcInputFilter;
_PERSISTENT static float adcSquareExpMovingMean[7] = {0,0,0,0,0,0,0};

void app_init() {
    adcInputFilter = filter_create(7, FilterTypeMovingMean, 4);
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
    
    float alpha = 1.0f/MB.N;
    MB.M0_AVG = MB.M0_value * alpha + (1.0f-alpha)*MB.M0_AVG;
    MB.M1_AVG = MB.M1_value * alpha + (1.0f-alpha)*MB.M1_AVG;
    MB.M2_AVG = MB.M2_value * alpha + (1.0f-alpha)*MB.M2_AVG;
    MB.A0_AVG = MB.A0_value * alpha + (1.0f-alpha)*MB.A0_AVG;
    MB.A1_AVG = MB.A1_value * alpha + (1.0f-alpha)*MB.A1_AVG;
    MB.A2_AVG = MB.A2_value * alpha + (1.0f-alpha)*MB.A2_AVG;
    MB.A3_AVG = MB.A3_value * alpha + (1.0f-alpha)*MB.A3_AVG;
    
    adcSquareExpMovingMean[0] = MB.M0_value * MB.M0_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[0];
    adcSquareExpMovingMean[1] = MB.M1_value * MB.M1_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[1];
    adcSquareExpMovingMean[2] = MB.M2_value * MB.M2_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[2];
    adcSquareExpMovingMean[3] = MB.A0_value * MB.A0_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[3];
    adcSquareExpMovingMean[4] = MB.A1_value * MB.A1_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[4];
    adcSquareExpMovingMean[5] = MB.A2_value * MB.A2_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[5];
    adcSquareExpMovingMean[6] = MB.A3_value * MB.A3_value * alpha + (1.0f - alpha)*adcSquareExpMovingMean[6];
    
    MB.M0_RMS = sqrt(adcSquareExpMovingMean[0]);
    MB.M1_RMS = sqrt(adcSquareExpMovingMean[1]);
    MB.M2_RMS = sqrt(adcSquareExpMovingMean[2]);
    MB.A0_RMS = sqrt(adcSquareExpMovingMean[3]);
    MB.A1_RMS = sqrt(adcSquareExpMovingMean[4]);
    MB.A2_RMS = sqrt(adcSquareExpMovingMean[5]);
    MB.A3_RMS = sqrt(adcSquareExpMovingMean[6]);
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
