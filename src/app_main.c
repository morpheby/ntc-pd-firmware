

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

enum ControlFlags {
    ctrlCalibrate = 1,
};

static int speed = 0;
static uint32_t lastRotDetection = 0;
static _Bool motorActive = 0;

#define ROT_TIMEOUT 100

void app_init() {
}

MAIN_DECL_LOOP_FN() {
    if (MB.Control0 & ctrlCalibrate) {
        MB.D_Out = 1;
        lastRotDetection = timing_get_time_msecs();
        motorActive = 1;
        MB.Control0 &= ~ctrlCalibrate;
    }
    if (motorActive && lastRotDetection + ROT_TIMEOUT < timing_get_time_msecs()) {
        speed = 0;
        motorActive = 0;
        MB.D_Out = 0;
    }
    
    // Set outputs from modbus
    discrete_set_output(MB.D_Out);
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    uint32_t nt = timing_get_time_msecs();
    speed = nt - lastRotDetection;
    lastRotDetection = nt;
}
