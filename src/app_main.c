

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
_PERSISTENT static Filter *forceValueFilter;
_PERSISTENT static Filter *positionInputFilter;

void app_init() {
    if (reset_is_cold()) {
        //set up default values
        adcInputFilter = filter_create(ADC_CHANNEL_COUNT,
                FilterTypeMovingMean, 10);
        forceValueFilter = filter_create(1,
                FilterTypeMovingMedian, 5);
        positionInputFilter = filter_create(1,
                FilterTypeMovingMedian, 15);
    }
    initPWM();
    set_PWM_output_inverted(0);
}

MAIN_DECL_LOOP_FN() {
    
    MB.D_In = discrete_get_input();
         
  /*  discrete_set_output(MB.Control0);
    PWM_on(MB.Power0);*/
    
    if(discrete_get_output() == 0x00) {
        PWM_off();
    } else {
        //PWM_on(4096);
       PWM_on(MB.Power0);
    }
    
    if(MB.Control0 == 0x01 || MB.Control0 == 0x02) {
        if(discrete_get_output() != MB.Control0) {
            if(discrete_get_output() == 0x00) {
                discrete_set_output(MB.Control0);
            } else {
                PWM_off();
                discrete_set_output(0x00);
            }
        }
    } else {
        PWM_off();
        discrete_set_output(0x00);
    }
    
    MB.A0 = filter_get(adcInputFilter, 0);
    filter_put(forceValueFilter, (MB.A0 - MB.OFS_ADC0), 0);
    MB.ADC0 = filter_get(forceValueFilter, 0)*MB.K0;
    MB.Position0 = filter_get(positionInputFilter, 0);
    /*            
    if((discrete_get_output()==0x00)) {
        //output is set to zero, motor is stopped. Switch off PWM
        PWM_off();
    } else {
        //motor is on. Switch on PWM
        PWM_on(MB.Power0);
    }
        
    if (MB.D_Out & 0x80) {
        // Override mode
        discrete_set_output(MB.D_Out);
    }*/
}

CNI_DECL_PROC_FN(29, on) {
    static uint32_t value = 0;
    static int counter = 0;
    static long int last_time = 0;
    
    if (!on && (counter || timing_get_time_msecs() > last_time+24)) {
        value |= discrete_get_input_bit(3) << counter;
        ++counter;
        if (counter == 24) {
            value = (~value) & 0x0FFFFFFFL;
            filter_put(positionInputFilter, value, 0);
            value = 0;
            counter = 0;
        }
	}

    if (!on) {
        last_time = timing_get_time_msecs();
    }
}

ADC_DECL_VALUE_FN(channel, value) {
    filter_put(adcInputFilter, value, channel);
}
