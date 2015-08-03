

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

void app_init() {
    if (reset_is_cold()) {
        //set up default values
    }
    initQEI();
    initPWM();
}

MAIN_DECL_LOOP_FN() {
    MB.D_In = discrete_get_input();
    MB.QEI_POS = POSCNT;
    disp_puti(0, MB.Position0);
    
    //0 - switch off
    //1 - 10 to output
    //11 - 01 to output
    PWM_on(MB.Power0);
        
    uint16_t controlFlag = MB.Control0;
    if ((controlFlag & 0x03) == 0x03) {
        discrete_set_output_bit(0, 1);
        discrete_set_output_bit(1, 0);
    } else if((controlFlag & 0x01) == 0x01) {
        discrete_set_output_bit(0, 0);
        discrete_set_output_bit(1, 1);
    }  else if((controlFlag & 0x80) == 0x80) {   
        discrete_set_output(0);
        PWM_off();
    }
    
    MB.Control0 = 0;
    
    if (MB.D_Out & 0x80) {
        // Override mode
        discrete_set_output(MB.D_Out);
    }
}

CNI_DECL_PROC_FN(29, on) {
    static uint32_t value = 0;
    static int counter = 0;
    static long int last_time = 0xEFFFFFFFL;
    
    if (!on && (counter || timing_get_time_low() > last_time+150000000L)) {
        value |= discrete_get_input_bit(3) << counter;
        ++counter;
        if (counter == 24) {
            value = (~value) & 0x0FFFFFFFL;
            MB.Position0 = value;
            value = 0;
            counter = 0;
        }
	}

    if (!on) {
        last_time = timing_get_time_low();
    }
}
