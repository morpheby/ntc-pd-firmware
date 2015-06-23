

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

enum ControlFlags {
    ctrlCalibrate = 1,
};

extern int Control;
extern int D_Out;

static int speed = 0;
static uint32_t lastRotDetection = 0;

#define ROT_TIMEOUT 100

void app_init() {
}

MAIN_DECL_LOOP_FN() {
    if (Control & ctrlCalibrate) {
        D_Out = 1;
        lastRotDetection = timing_get_time_msecs();
        Control &= ~ctrlCalibrate;
    }
    if (lastRotDetection + ROT_TIMEOUT < timing_get_time_msecs()) {
        speed = 0;
        
        D_Out = 0;
    }
    
    // Set outputs from modbus
    discrete_set_output(D_Out);
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    uint32_t nt = timing_get_time_msecs();
    speed = nt - lastRotDetection;
    lastRotDetection = nt;
}
