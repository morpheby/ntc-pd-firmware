#include <string.h>

#include "ind_profiles.h"
#include "system.h"
#include "display.h"
#include "menu-base.h"
#include "modbus_registers.h"
#include "D_I_O.h"

#define REG_TYPE_INT 0
#define REG_TYPE_UINT 1
#define REG_TYPE_FLOAT 2

static uint16_t indDelayCounter = 0;

void setDisplayData(uint8_t dispNum, uint16_t profile);

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
        setDisplayData(2, ind2_profile);        
    }    
}

void setDisplayData(uint8_t dispNum, uint16_t profile)
{
    uint8_t regNumber = profile >> 8;
    uint8_t reg_type = (profile & 0x00F0)>>4;
    
    if(regNumber < 0xFF) { //0xFF(255) is used as indicator switch off command
        disp_set_off(0,0);
        if(reg_type == REG_TYPE_FLOAT) {
            uint8_t point = profile & 0x000F;
            float value = modbus_get_reg_f(regNumber);
            disp_fix_point(dispNum, adaptPrecision(value, point));
            disp_putf(0, value);        
        } else if(reg_type == REG_TYPE_UINT)
        {
            disp_fix_point(dispNum, 0);
            disp_putu(dispNum, modbus_get_reg_u16(regNumber));
        }else
        {
            disp_fix_point(dispNum, 0);
            disp_puti(dispNum, modbus_get_reg_i16(regNumber));
        }        
    } else {
        disp_set_off(dispNum,1);
    }
}

uint8_t adaptPrecision(float value, uint8_t precision)
{
    uint8_t result = precision;
    if(precision > 0) {
        switch(precision) {
            case 1: {
                if(value > 999 || value < -99) {
                    result-=1;
                }
                break;
            }
            case 2 : {
                if(value > 99 || value < -9) {
                    result = adaptPrecision(value, result-1);
                }
                break;
            }
            case 3 : {
                if(value > 9 || value < 0) {
                    result = adaptPrecision(value, result-1);
                }
                break;
            }
        }
    }
    return result;
}