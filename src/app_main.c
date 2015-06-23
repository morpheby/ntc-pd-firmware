
#include "system.h"
#include "app_main.h"
#include "app_connector.h"
#include "board-config.h"

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

void app_init() {
    
}

MAIN_DECL_LOOP_FN() {
    
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    
}

DIO_DECL_UPDATE_FN(dOutValue) {
   // Switch latches for discrete outputs
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = (_Bool)(dOutValue&1);
    PIN_LATCH(VT1_PIN_TYPE, VT1_PIN_NUM) = (_Bool)(dOutValue&2);
    PIN_LATCH(VT2_PIN_TYPE, VT2_PIN_NUM) = (_Bool)(dOutValue&4);
    PIN_LATCH(VT3_PIN_TYPE, VT3_PIN_NUM) = (_Bool)(dOutValue&8);
}