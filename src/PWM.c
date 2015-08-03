#include "PWM.h"
#include "system.h"

static bool outputInverted;

void initPWM()
{
    PWM_off();
    P2TCONbits.PTEN = 1;
    P2TCONbits.PTCKPS = 0b10;
    P2TPER = 2048;
    PWM2CON1bits.PMOD1 = 1;
    PWM2CON1bits.PEN1L = 1;
    outputInverted = 0;
}

void PWM_on(uint16_t value)
{
    if(outputInverted){
        P2DC1 = 4096 - value;                  
    } else {
        P2DC1 = value;        
    }
    P2OVDCONbits.POVD1L = 1;
}

void PWM_off()
{
    P2OVDCONbits.POVD1L = 0;
}


void set_PWM_output_inverted(bool flag)
{
    outputInverted = flag;
}