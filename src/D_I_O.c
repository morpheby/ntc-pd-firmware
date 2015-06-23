
#include "D_I_O.h"
#include "system.h"
#include "board-config.h"
#include "app_connector.h"
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
    DIO_CALL_UPDATE_FN(D_Out);
}
