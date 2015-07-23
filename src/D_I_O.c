
#include "D_I_O.h"
#include "system.h"
#include "board-config.h"
#include "app_connector.h"
#include "timing.h"

static uint16_t _doutValue;
static uint16_t _dinValue;

DIO_DECL_UPDATE_FN(a,b,c);

void discrete_init() {
    TRIS_BIT(DI0_PIN_TYPE, DI0_PIN_NUM) = 1;
    TRIS_BIT(DI1_PIN_TYPE, DI1_PIN_NUM) = 1;
    TRIS_BIT(DI2_PIN_TYPE, DI2_PIN_NUM) = 1;
    TRIS_BIT(DI3_PIN_TYPE, DI3_PIN_NUM) = 1;
    
    TRIS_BIT(VT0_PIN_TYPE, VT0_PIN_NUM) = 0;
    TRIS_BIT(VT1_PIN_TYPE, VT1_PIN_NUM) = 0;
    TRIS_BIT(VT2_PIN_TYPE, VT2_PIN_NUM) = 0;
    TRIS_BIT(VT3_PIN_TYPE, VT3_PIN_NUM) = 0;
}

void discrete_set_output(uint16_t value) {
    _doutValue = value;
}

uint16_t discrete_get_output() {
    return _doutValue;
}

void discrete_set_output_bit(_Bool output, uint16_t idx) {
    uint16_t v = 1 << idx;
    uint16_t out = discrete_get_output();
    if (output) {
        out |= v;
    } else {
        out &= ~v;
    }
    discrete_set_output(out);
}

_Bool discrete_get_output_bit(uint16_t idx) {
    return (_Bool)(discrete_get_output() & (1 << idx));
}

uint16_t discrete_get_input() {
    return _dinValue;
}

_Bool discrete_get_input_bit(uint16_t idx) {
    return (_Bool)(discrete_get_input() & (1 << idx));
}

void discrete_sample() {
    _dinValue = (PIN_PORT(DI0_PIN_TYPE, DI0_PIN_NUM) << 0) |
                (PIN_PORT(DI1_PIN_TYPE, DI1_PIN_NUM) << 1) |
                (PIN_PORT(DI2_PIN_TYPE, DI2_PIN_NUM) << 2) |
                (PIN_PORT(DI3_PIN_TYPE, DI3_PIN_NUM) << 3);
}

void discrete_latch() {
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = (_Bool)(_doutValue&1);
    PIN_LATCH(VT1_PIN_TYPE, VT1_PIN_NUM) = (_Bool)(_doutValue&2);
    PIN_LATCH(VT2_PIN_TYPE, VT2_PIN_NUM) = (_Bool)(_doutValue&4);
    PIN_LATCH(VT3_PIN_TYPE, VT3_PIN_NUM) = (_Bool)(_doutValue&8);
}

void discrete_update() {
    // Read input values
    uint16_t dinValueOld = _dinValue;
    discrete_sample();
    
    // If application requires different values, it may update them here
    DIO_CALL_UPDATE_FN(_dinValue, dinValueOld, &_doutValue);
    
    // Write output values
    discrete_latch();
}
