#include <string.h>

#include "ind_profiles.h"
#include "system.h"
#include "display.h"
#include "modbus_registers.h"
#include "D_I_O.h"
#include "util.h"

#define REG_TYPE_INT 0
#define REG_TYPE_UINT 1
#define REG_TYPE_FLOAT 2

static uint16_t indDelayCounter = 0;
static char dispBuff[DISPLAY_COUNT+1];

static int dispMaxValue = 9999;
static int dispMinValue = -999;
static int maxDisplayPrecision = CHARS_IN_SECTION - 1;

void setDisplayData(uint8_t dispNum, uint16_t profile);
void displayNumber(uint8_t dispNum, int displayableValue, int displayablePrecision);

void ind_init()
{
    dispMaxValue = 9;
    int i;
    for(i = 1; i < CHARS_IN_SECTION; ++i) {
        dispMaxValue = dispMaxValue * 10 + 9;
    }
    dispMinValue = -dispMaxValue / 10;
    
    for(i = 0; i < DISPLAY_COUNT; ++i) {
        dispBuff[i] = '-';
    }
    display_set(dispBuff);
}

void ind_showValues()
{
    if(indDelayCounter ++ < MB.Ind_Delay) {
        return;
    }
    indDelayCounter = 0;
    
    if(DISPLAY_COUNT == 0)
    {
        //no displays connected
        return;
    }
    
    // use first profile as default
    uint16_t ind0_profile = MB.ind_0_1;
    uint16_t ind1_profile = MB.ind_1_1;   
    uint16_t ind2_profile = MB.ind_2_1;  
    
    if(MB.PROF_CHANGE_SOURCE == 1) {      
        MB.profile = (discrete_get_input() & 0x03)+1;
    }
    
    uint8_t currentProfile = MB.profile;
    
    switch(currentProfile)
    {
        case 2: ind0_profile = MB.ind_0_2;
                ind1_profile = MB.ind_1_2;   
                ind2_profile = MB.ind_2_2; 
                break;
        case 3: ind0_profile = MB.ind_0_3;
                ind1_profile = MB.ind_1_3;   
                ind2_profile = MB.ind_2_3;
                break;
        case 4: ind0_profile = MB.ind_0_4;
                ind1_profile = MB.ind_1_4;   
                ind2_profile = MB.ind_2_4;
                break;
        default:
            MB.profile = 1;
    }
        
    // show value on display #0
    setDisplayData(0, ind2_profile);
    
    if(DISPLAY_COUNT/CHARS_IN_SECTION > 1) {
        //show value on display #1
        setDisplayData(1, ind1_profile);
    }
    
    if(DISPLAY_COUNT/CHARS_IN_SECTION > 2) {
        //show value on display #2
        setDisplayData(2, ind0_profile);        
    }  
    display_set(dispBuff);
}

void setDisplayData(uint8_t dispNum, uint16_t profile)
{
    uint8_t regNumber = profile >> 8;
    uint8_t reg_type = (profile & 0x00F0)>>4;
    
    if(regNumber < 0xFF) { //0xFF(255) is used as indicator switch off command
        if(reg_type == REG_TYPE_FLOAT) {
            uint8_t point = profile & 0x000F;
            float value = modbus_get_reg_f(regNumber);
            ind_putf(dispNum, value, point);
        } else if(reg_type == REG_TYPE_UINT)
        {
            ind_puti(dispNum, modbus_get_reg_u16(regNumber));
        }else
        {          
            ind_puti(dispNum, modbus_get_reg_i16(regNumber));
        }        
    } else {
        ind_set_off(dispNum);
    }
}

void ind_putf(uint8_t dispNum, float value, uint8_t precision)
{
    if(value > dispMaxValue) {
        value = dispMaxValue;
    }
    if(value < dispMinValue) {
        value = dispMinValue;
    }
    int displayablePrecision = 0;
    int displayableValue = (int)value;
    while(displayablePrecision < precision && displayablePrecision < maxDisplayPrecision) {
        int tmp = value * 10;
        if(tmp >= dispMinValue && tmp <= dispMaxValue) {
            value*=10;
            displayableValue = (int)(value);
            displayablePrecision++;
        } else {
            break;
        }
    }
    
    displayNumber(dispNum, displayableValue, displayablePrecision);
}

void ind_puti(uint8_t dispNum, int value)
{
    if(value > dispMaxValue) {
        value = dispMaxValue;
    }
    if(value < dispMinValue) {
        value = dispMinValue;
    }
    displayNumber(dispNum, value, 0);
}

void displayNumber(uint8_t dispNum, int displayableValue, int displayablePrecision)
{
    uint8_t pos = dispNum*CHARS_IN_SECTION;
    int i;
    int minusPos = -1;
    if(displayableValue < 0) {
        minusPos = 0;
        displayableValue = abs_fast(displayableValue);
    }
    int pointPos = pos + CHARS_IN_SECTION - displayablePrecision-1;
    for(i = CHARS_IN_SECTION - 1; i >=0; --i) {
        int valueNumber = displayableValue % 10;
        
        if(valueNumber >= 0 && (displayableValue > 0 || pos + i >= pointPos)) {
            dispBuff[pos+i] = valueNumber + '0';
        } else {
            dispBuff[pos+i] = ' ';
            if(minusPos == 0) {
                minusPos = pos+i;
            }
        }
        displayableValue /= 10;
    }
    if(displayablePrecision > 0) {
        dispBuff[pointPos] |= 1 << 7; // dot
    }
    if(minusPos >= 0) {
        dispBuff[pos + minusPos] = '-';
    }
}

void ind_set_off(uint8_t dispNum)
{
    int i;
    uint8_t pos = dispNum*CHARS_IN_SECTION;
    for(i = 0; i < CHARS_IN_SECTION; ++i) {
        dispBuff[i+pos] = '-';
    }
}