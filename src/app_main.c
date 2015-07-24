

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
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
    MB.QEI_POS = POSCNT;
    disp_puti(0, MB.Position0);
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
