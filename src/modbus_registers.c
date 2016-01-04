#include "modbus_registers.h"
#include "filter.h"

struct modbus_t MB;


void modbus_regs_init()
{
    uint16_t * mbDataPtr = &MB.ADDRESS;
    uint8_t i;
    for(i = 0; i < MB_REGS_COUNT; ++i) {
        mbDataPtr[i] = 0;
    }
}

float modbus_get_reg_f(uint8_t regNumber)
{
    return 0.0f;
}

uint16_t modbus_get_reg_u16(uint8_t regNumber)
{
    return 0;
}
int16_t modbus_get_reg_i16(uint8_t regNumber)
{
    return 0;
}