#include "PWM.h"
#include "system.h"

void initPWM()
{
    PWM_off();
    P2TCONbits.PTEN = 1;
    P2TCONbits.PTCKPS = 0b10;
    P2TPER = 2048;
    PWM2CON1bits.PMOD1 = 1;
    PWM2CON1bits.PEN1L = 1;
}

void PWM_on(uint16_t value)
{
    P2DC1 = value;
    P2OVDCONbits.POVD1L = 1;
}

void PWM_off()
{
    P2OVDCONbits.POVD1L = 0;
}
