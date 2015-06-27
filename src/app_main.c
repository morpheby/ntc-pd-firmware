

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

/*
 * This file contains functions, being called from every module upon some
 * events. Use this module for all application-specific tasks.
 */

enum ControlFlags {
    ctrlCalibrate = 0x0001,
    ctrlStartMeasure = 0x0002,
    ctrlCoilOn = 0x0004,
    ctrlCoilOff = 0x0008,
    ctrlMove = 0x0010,
    ctrlAbort = 0x8000,
};

enum StatusFlags {
    stMotorOn = 0x0001,
    stMotorReverse = 0x0002,
    stNeedsCalibration = 0x0004,
    stMeasureMode = 0x0008,
    stCoilOn = 0x0010,
    stError = 0x8000,
};

#define ADC_BUFFER_SIZE 128

static float speed = 0;
static _time_t lastRotDetection = {0,0};
static _PERSISTENT int impulseCounter;
static _PERSISTENT float *adcData;
static _PERSISTENT int adcDataSz;


static const _time_t ROT_TIMEOUT = {0, 100000000L};

static void motor_start(_Bool reverse) {
    MB.Status0 |= stMotorOn;
    MB.Status0 |= reverse ? stMotorReverse : 0;
    if (speed == 0) {
        speed = FLT_MIN;
        lastRotDetection = timing_get_time();
    }
}

static void motor_stop() {
    MB.Status0 &= ~(stMotorOn|stMotorReverse);
}

static void measure_start() {
    if (!adcData) {
        adcData = gc_malloc(ADC_BUFFER_SIZE*sizeof(float));
    }
    MB.Status0 |= stMeasureMode;
    adcDataSz = 0;
    modbus_mmap_free();
}

static void measure_stop() {
    MB.Status0 &= ~stMeasureMode;
}

void app_gc() {
    measure_stop();
    modbus_mmap_free();
    if (adcData) {
        gc_free(adcData);
        adcData = 0;
    }
}

void app_init() {
    if (reset_is_cold()) {
        garbage_collect_reg(app_gc);
        adcDataSz = 0;
        adcData = 0;
        impulseCounter = 0;
        MB.Status0 = 0;
        MB.Control0 = 0;
        MB.Status0 |= stNeedsCalibration;
    }
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
            if (!(MB.Status0 & stMotorReverse)) {
                // Calibration finished
                MB.Status0 &= ~stNeedsCalibration;
                impulseCounter = 0;
            }
            motor_stop();
        }
        if (MB.Status0 & stMeasureMode) {
            measure_stop();
        }
    } else {
        if (!(MB.Status0 & stMotorOn)) {
            // Unexpected movement, mark invalid position
            if (impulseCounter >= 4) {
                MB.Status0 |= stNeedsCalibration;
            }
        }
    }
    
    // Read commands
    if (MB.Control0 & ctrlCalibrate) {
        motor_start(false);
        impulseCounter = INT16_MAX;
    }
    
    // Update linear position
    MB.Position1 = impulseCounter * MB.PositionK0;
    
    if (MB.Control0 & ctrlMove) {
        motor_start(MB.Position0 > MB.Position1);
    }
    
    if (MB.Control0 & ctrlStartMeasure) {
        if (MB.Status0 & stMotorOn) {
            MB.Status0 |= stError;
        } else {
            measure_start();
        }
    }
    
    if (MB.Control0 & ctrlCoilOn) {
        MB.Status0 |= stCoilOn;
    }
    
    if (MB.Control0 & ctrlCoilOff) {
        MB.Status0 &= ~stCoilOn;
    }
    
    // Stop when requested position is reached
    if ((MB.Position0 <= MB.Position1 && MB.Status0 & stMotorReverse)
     || (MB.Position0 >= MB.Position1 && !(MB.Status0 & stMotorReverse))) {
        motor_stop();
    }
    
    if (MB.Control0 & ctrlAbort) {
        // Stop all operations
        MB.Status0 = 0;
        // Clear D_Out
        MB.D_Out = 0;
        // Zero speed
        speed = 0;
        // Reset counters
        adcDataSz = 0;
        impulseCounter = 0;
        // Reset modbus_mmap
        modbus_mmap_free();
        
        // Actually, status is need calibrarion, so
        MB.Status0 |= stNeedsCalibration;
    }
    
    // Zero command register to acknowledge command reception
    MB.Control0 = 0;
    
    // Read external direct controls from modbus
    // NOTE: use bit 15 to mark override mode
    if (MB.D_Out & 0x8000) {
        discrete_set_output(MB.D_Out);
    } else {
        discrete_set_output_bit(MB.Status0 & stMotorReverse, REVERSE);
        discrete_set_output_bit(MB.Status0 & stMotorOn, MOTOR);
        discrete_set_output_bit(MB.Status0 & stCoilOn, COIL);
        
        MB.D_Out = discrete_get_output();
    }
}

CNI_DECL_PROC_FN(CNI_DETECTOR, on) {
    update_speed();
    
    if (MB.Status0 & stMotorReverse) {
        ++impulseCounter;
    } else {
        --impulseCounter;
    }
    if ((MB.Status0 & stMeasureMode) && !(impulseCounter % 6)) {
        adcData[adcDataSz++] = MB.ADC3;
        if (adcDataSz == ADC_BUFFER_SIZE) {
            // We cannot hold more
            MB.Status0 |= stError;
            measure_stop();
        }
        modbus_mmap_set(adcData, adcDataSz*sizeof(float)/sizeof(uint16_t));
    }
}
