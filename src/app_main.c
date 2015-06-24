

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

enum ControlFlags {
    ctrlCalibrate = 0x01,
};

enum StatusFlags {
    stMotorOn = 0x01,
    stMotorReverse = 0x02,
};

static float speed = 0;
static _time_t lastRotDetection = {0,0};

static const _time_t ROT_TIMEOUT = {0, 100000000L};

void app_init() {
}

static void motor_start() {
    MB.Status0 |= stMotorOn;
    if (speed == 0) {
        speed = FLT_MIN;
        lastRotDetection = timing_get_time();
    }
}

static void motor_stop() {
    MB.Status0 &= ~stMotorOn;
}

void update_speed() {
    _time_t nt = timing_get_time();
    _time_t diff = nt;
    time_sub(&diff, lastRotDetection);
    
    speed = ((double)diff.nsecs) + ((double)diff.secs) * 1e+9;
    lastRotDetection = nt;
}

MAIN_DECL_LOOP_FN() {
    _time_t timeout = lastRotDetection;
    time_add(&timeout, ROT_TIMEOUT);
    if (time_compare(timeout, timing_get_time()) < 0) {
        // Zero speed on timeout
        speed = 0;
    }
    
    if (speed == 0) {
        if (MB.Status0 & stMotorOn) {
            motor_stop();
        }
    } else {
        if (!(MB.Status0 & stMotorOn)) {
            // Unexpected movement, start calibration
            motor_start();
        }
    }
    
    // Read commands
    if (MB.Control0 & ctrlCalibrate) {
        motor_start();
    }
    
    // Zero command register to acknowledge command reception
    MB.Control0 = 0;
    
    // Read external direct controls from modbus
    // NOTE: use bit 15 to mark override mode
    if (MB.D_Out & 0x80) {
        discrete_set_output(MB.D_Out);
    } else {
        discrete_set_output_bit(MB.Status0 & stMotorReverse, REVERSE);
        discrete_set_output_bit(MB.Status0 & stMotorOn, MOTOR);
    }
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    update_speed();
}
