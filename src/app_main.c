

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

void app_init() {
    if (reset_is_cold()) {
        //set up default values
    }
    initQEI();
}

MAIN_DECL_LOOP_FN() {
    discrete_set_output(MB.D_Out);
    MB.D_In = discrete_get_input();
    MB.QEI_POS = POSCNT;
    disp_puti(0, POSCNT);
}
