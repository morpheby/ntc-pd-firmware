

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
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    uint32_t nt = timing_get_time_msecs();
    speed = nt - lastRotDetection;
    lastRotDetection = nt;
}

DIO_DECL_UPDATE_FN(dOutValue) {
   // Switch latches for discrete outputs
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = (_Bool)(dOutValue&1);
    PIN_LATCH(VT1_PIN_TYPE, VT1_PIN_NUM) = (_Bool)(dOutValue&2);
    PIN_LATCH(VT2_PIN_TYPE, VT2_PIN_NUM) = (_Bool)(dOutValue&4);
    PIN_LATCH(VT3_PIN_TYPE, VT3_PIN_NUM) = (_Bool)(dOutValue&8);
}