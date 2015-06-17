
#include "D_I_O.h"
#include "system.h"
#include "board-config.h"
#include "cn_inputs_reg.h"
#include "timing.h"

extern int D_In;
extern int D_Out;

void DI_Init() {
    TRIS_BIT(DI0_PIN_TYPE, DI0_PIN_NUM) = 1;
    TRIS_BIT(DI1_PIN_TYPE, DI1_PIN_NUM) = 1;
    TRIS_BIT(DI2_PIN_TYPE, DI2_PIN_NUM) = 1;
    TRIS_BIT(DI3_PIN_TYPE, DI3_PIN_NUM) = 1;
    
    TRIS_BIT(VT0_PIN_TYPE, VT0_PIN_NUM) = 0;
    TRIS_BIT(VT1_PIN_TYPE, VT1_PIN_NUM) = 0;
    TRIS_BIT(VT2_PIN_TYPE, VT2_PIN_NUM) = 0;
    TRIS_BIT(VT3_PIN_TYPE, VT3_PIN_NUM) = 0;
}

void discrete_update() {
   // Switch latches for discrete outputs
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = (_Bool)(D_Out&1);
    PIN_LATCH(VT1_PIN_TYPE, VT1_PIN_NUM) = (_Bool)(D_Out&2);
    PIN_LATCH(VT2_PIN_TYPE, VT2_PIN_NUM) = (_Bool)(D_Out&4);
    PIN_LATCH(VT3_PIN_TYPE, VT3_PIN_NUM) = (_Bool)(D_Out&8);
}

CNI_DECL_PROC_FN(CNI_DI3) {
}
