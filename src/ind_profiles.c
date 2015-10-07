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
        MB.profile = discrete_get_input();
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
    
    uint8_t ind0_regNumber = ind0_profile >> 8;
    uint8_t ind1_regNumber = ind1_profile >> 8;
    uint8_t ind2_regNumber = ind2_profile >> 8;
    
    uint8_t ind0_reg_type = (ind0_profile & 0x00F0)>>4;
    uint8_t ind1_reg_type = (ind1_profile & 0x00F0)>>4;
    uint8_t ind2_reg_type = (ind2_profile & 0x00F0)>>4;
    
    // show value on display #0
    if(ind2_regNumber < 0xFF) { //0xFF(255) is used as indicator switch off command
        disp_set_off(0,0);
        if(ind2_reg_type == REG_TYPE_FLOAT) {
            uint8_t ind2_point = ind2_profile & 0x000F;
            disp_fix_point(0, ind2_point);
            disp_putf(0, modbus_get_reg_f(ind2_regNumber));        
        } else if(ind2_reg_type == REG_TYPE_UINT)
        {
            disp_fix_point(0, 0);
            disp_putu(0, modbus_get_reg_u16(ind2_regNumber));
        }else
        {
            disp_fix_point(0, 0);
            disp_puti(0, modbus_get_reg_i16(ind2_regNumber));
        }        
    } else {
        disp_fix_point(0, 0);
        disp_set_off(0,1);
    }
    
    if(DISPLAY_COUNT/CHARS_IN_SECTION > 1) {
        //show value on display #1
        if(ind1_regNumber < 0xFF) {//0xFF(255) is used as indicator switch off command
            disp_set_off(1,0);
            if(ind1_reg_type == REG_TYPE_FLOAT) {
                uint8_t ind1_point = ind1_profile & 0x000F;
                disp_fix_point(1, ind1_point);
                disp_putf(1, modbus_get_reg_f(ind1_regNumber));        
            }else if(ind1_reg_type == REG_TYPE_UINT)
            {
                disp_fix_point(1, 0);
                disp_putu(1, modbus_get_reg_u16(ind1_regNumber));
            } else 
            {
                disp_fix_point(1, 0);
                disp_puti(1, modbus_get_reg_i16(ind1_regNumber));
            }
        } else {
            disp_fix_point(0, 0);
            disp_set_off(1,1);
        }        
    }
    
    if(DISPLAY_COUNT/CHARS_IN_SECTION > 2) {
        //show value on display #2
        if(ind0_regNumber < 0xFF) {//0xFF(255) is used as indicator switch off command  
            disp_set_off(2,0);     
            if(ind0_reg_type == REG_TYPE_FLOAT) {
                uint8_t ind0_point = ind0_profile & 0x000F;
                disp_fix_point(2, ind0_point);
                disp_putf(2, modbus_get_reg_f(ind0_regNumber));        
                }else if(ind0_reg_type == REG_TYPE_UINT)
            {
                disp_fix_point(2, 0);
                disp_putu(2, modbus_get_reg_u16(ind0_regNumber));
            } else 
            {
                disp_fix_point(2, 0);
                disp_puti(2, modbus_get_reg_i16(ind0_regNumber));
            }
        } else {
            disp_fix_point(2, 0);
            disp_set_off(2,1);
        }        
    }    
}
