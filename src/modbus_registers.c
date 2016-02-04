#include <string.h>

#include "modbus_registers.h"

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
    float result;
    uint16_t* resultDataPtr = &result;
    uint16_t* mbDataPtr = &MB.ADDRESS;
    resultDataPtr[0] = mbDataPtr[regNumber];
    resultDataPtr[1] = mbDataPtr[regNumber+1];
    return result;
}

uint16_t modbus_get_reg_u16(uint8_t regNumber)
{
    uint16_t* mbData = &MB.ADDRESS;
    return mbData[regNumber];
}
int16_t modbus_get_reg_i16(uint8_t regNumber)
{
    int16_t* mbData = &MB.ADDRESS;
    return mbData[regNumber];
}