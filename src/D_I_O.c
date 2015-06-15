
#include "D_I_O.h"
#include "system.h"
#include "board-config.h"
#include "cn_inputs_reg.h"

extern int D_In;
extern int count_12_5us;

void DI_Init() {
    TRIS_BIT(DI0_PIN_TYPE, DI0_PIN_NUM) = 1;
    TRIS_BIT(DI1_PIN_TYPE, DI1_PIN_NUM) = 1;
    TRIS_BIT(DI2_PIN_TYPE, DI2_PIN_NUM) = 1;
    TRIS_BIT(DI3_PIN_TYPE, DI3_PIN_NUM) = 1;
}

unsigned char DI() {
   return 0;
}

CNI_DECL_PROC_FN(CNI_DI3) {
    static uint32_t value = 0;
    static int counter = 0;
    static int last_time = 0xEFFD;
    int i;
    
    if (!__on && (counter || count_12_5us > last_time+10)) {
        value |= ((uint32_t)PIN_PORT(DI0_PIN_TYPE, DI0_PIN_NUM) & 1) << counter;
        ++counter;
        if (counter == 24) {
            value = (~value) & 0x0FFFFFFFL;
            D_In = value;
            value = 0;
            counter = 0;
        }
    }
    
    if (!__on) {
        last_time = count_12_5us;
    }
}
