
#define MODBUS_INTERNAL

#include "modbus_registers.h"
#include <string.h>

MODBUS _PERSISTENT _ModbusData_t MB;
static uint16_t *_modbusData = MB.data;

#if APP_USE_MODBUS_EXT
static _PERSISTENT uint16_t *_modbusMMapData;
static _PERSISTENT uint16_t  _modbusMMapSize;
#endif

uint16_t *modbus_get_addr(uint16_t offset);


void modbus_init() {
    if (reset_is_cold()) {
        memset(_modbusData, 0, MODBUS_DATA_END*sizeof(uint16_t));
#if APP_USE_MODBUS_EXT
        _modbusMMapData = 0;
        _modbusMMapSize = 0;
#endif
    }
}

uint16_t *modbus_get_addr(uint16_t offset) {
    if (0 <= offset && offset < MODBUS_DATA_END) {
        return _modbusData + offset;
#if APP_USE_MODBUS_EXT
    } else if (offset == REG_OFFSET(REG_Bulk_Size)) {
        return &_modbusMMapSize;
    } else if (MODBUS_BULK_START <= offset
            && offset < MODBUS_BULK_START + _modbusMMapSize) {
        return _modbusMMapData + (offset - MODBUS_BULK_START);
#endif
    } else {
        return 0;
    }
}

void modbus_get_reg_data(uint16_t offset, uint16_t count, uint16_t *out) {
    uint16_t i;
    for (i = offset; i < offset + count; ++i) {
        *out = *modbus_get_addr(i);
    }
}

void modbus_set_reg_data(uint16_t offset, uint16_t count, const uint16_t *in) {
    uint16_t i;
    for (i = offset; i < offset + count; ++i) {
        *modbus_get_addr(i) = *in;
    }
}

#define _MB_GET_REG_FN(t)                                       \
_MB_GET_REG_DECL(t) {                                           \
    _MB_TYPE(t) v;                                              \
    modbus_get_reg_data(offset, _MB_SIZE(t), (uint16_t *)&v);   \
    return v;                                                   \
}

_MB_GET_REG_FN(i16)
_MB_GET_REG_FN(u16)
_MB_GET_REG_FN(f)

#define _MB_SET_REG_FN(t)                                           \
_MB_SET_REG_DECL(t) {                                               \
    modbus_set_reg_data(offset, _MB_SIZE(t), (uint16_t *)&value);   \
}

_MB_SET_REG_FN(i16)
_MB_SET_REG_FN(u16)
_MB_SET_REG_FN(f)

        
#if APP_USE_MODBUS_EXT

void modbus_mmap_set(void *ptr, uint16_t size) {
    _modbusMMapData = ptr;
    _modbusMMapSize = size;
}

void modbus_mmap_free() {
    _modbusMMapSize = 0;
    _modbusMMapData = 0;
}

#endif
