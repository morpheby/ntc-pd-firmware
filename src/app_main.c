

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

#include "quadrature_encoder.h"
#include  "menu-base.h"
#include "PWM.h"
#include "ind_profiles.h"
#include "filter.h"

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        setFilterType(_movingMeanFilterType);
    }
    initQEI();
    initPWM();
}

MAIN_DECL_LOOP_FN() {
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
    
    // Process ADC values
    MB.ADC0 = (MB.A0 - MB.OFS_ADC0)*MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1)*MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2)*MB.K2;
    MB.ADC3 = (MB.A3 - MB.OFS_ADC3)*MB.K3;
    MB.ADC4 = (MB.A4 - MB.OFS_ADC4)*MB.K4;
    MB.ADC5 = (MB.A5 - MB.OFS_ADC5)*MB.K5;
    MB.ADC6 = (MB.A6 - MB.OFS_ADC6)*MB.K6;
//        
//    ADC4Sum += MB.ADC4;
//    ADC5Sum += MB.ADC5;
//    ADC6Sum += MB.ADC6;
//    
//    ADC4SumSq += (MB.ADC4*MB.ADC4);
//    ADC5SumSq += (MB.ADC5*MB.ADC5);
//    ADC6SumSq += (MB.ADC6*MB.ADC6);
//    
//    iterationsCount++;
//    if(iterationsCount>MB.N)
//    {
//        MB.AD4_AVG = ADC4Sum / MB.N;
//        MB.AD5_AVG = ADC5Sum / MB.N;
//        MB.AD6_AVG = ADC6Sum / MB.N;
//                
//        MB.AD4_RMS = sqrt(ADC4SumSq / MB.N);
//        MB.AD5_RMS = sqrt(ADC5SumSq / MB.N);
//        MB.AD6_RMS = sqrt(ADC6SumSq / MB.N);
//        
//        iterationsCount = 0;
//        ADC4Sum = 0;
//        ADC5Sum = 0;
//        ADC6Sum = 0;
//        
//        ADC4SumSq = 0;
//        ADC5SumSq = 0;
//        ADC6SumSq = 0;
//    }
}
