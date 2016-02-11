/******************************************************************************/

#include <string.h>

#include "system.h"

/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "oscillators.h"
#include "uart_base.h"
#include "wdt.h"
#include "cn_inputs.h"
#include "mem_pool.h"
#include "timing.h"
#include "app_connector.h"
#include "display.h"

#include "modbus.h"
#include "ADC.h"
#include "math.h"
#include "D_I_O.h"
#include "modbus_registers.h"
#include "ind_profiles.h"
#include "DEE Emulation 16-bit.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
#define DEFAULT_IND_PROFILE 0xFF00

void loadParams();
void saveParams();

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main() {    
    /* Initialize reset parser */
    system_init();
    
    wdt_init();

    mempool_init();
    
    cpu_ipl_set(0);
    
    /* Initialize display */
    disp_init();
    
    ind_init();
    
    /* Initialize Change-notification inputs */
    cni_init();
    
    /* Initialize oscillator switcher */
    osc_init();

    // Select Internal FRC with PLL
    osc_select(oscBoosted);
        
    /* Initialize modbus registers */
    modbus_regs_init();
    
    loadParams();
    
    /* Initialize UART */
    uart_init();
        
    /* Initialize system timing */
    timing_init();
            
    ADC_Init(1);
    discrete_init();
           
    setModbusAddress(MB.ADDRESS);
    float periode = 10.0/MB.BAUD_RATE*3.5;
    set_mb_silence_timer_periode((unsigned int)periode+1);
    
    MB.D_Out = MB.D_OutInit;
    
    MB.LCD_buf_size = FORMAT_STRING_BUF_SIZE;
        
    app_init();
    
    // Main cycle
    while (1) {             
        display_update(0);
        discrete_update();
        perform_data_operations();
                                   
        if(MB.FLASH_WRITE == 1) {
            
            SYSHANDLE hp = high_priority_enter();
            saveParams();
            high_priority_exit(hp);
            
            MB.FLASH_WRITE = 0;
        }
        
        if(MB.RESET == 1) {
            system_reset();
            MB.RESET = 0;
        }
                            
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}

#define FLASH_FIRST_START_FLAG_ADDR 0
#define FLASH_MB_ADDRESS_ADDR 1
#define FLASH_BAUD_RATE_ADDR 2
#define FLASH_ADC_OFFSET_ADDR 4
#define FLASH_ADC_COEF_ADDR 11
#define FLASH_N_ADDR 25
#define FLASH_DOUT_INIT_ADDR 26
#define FLASH_DI_IMP_FREQ_COEF_ADDR 27
#define FLASH_DISK_IMP_COUNT_ADDR 35
#define FLASH_IND_DELAY_ADDR 36
#define FLASH_PROF_CHANGE_SOURCE_ADDR 37
#define FLASH_PROFILES_ADDR 38
#define FLASH_SIGN_THRESH_ADDR 51
#define FLASH_POWER_COEF_ADDR 65
#define FLASH_LCD_ROWS_ADDR 71
#define FLASH_LCD_COLS_ADDR 72
#define FLASH_LCD_FORMAT_ADDR 73

void saveParams()
{
    int i;
    int *offsetPtr = &MB.M0_OFFSET;    
    unsigned int *tmpPtr;
    
    DataEEWrite(MB.ADDRESS, FLASH_MB_ADDRESS_ADDR); 
    
    tmpPtr = &MB.BAUD_RATE;
    DataEEWrite(tmpPtr[0], FLASH_BAUD_RATE_ADDR); 
    DataEEWrite(tmpPtr[1], FLASH_BAUD_RATE_ADDR + 1); 
    
    for(i = 0; i < 7; ++i) {
        DataEEWrite(offsetPtr[i], FLASH_ADC_OFFSET_ADDR + i); 
        
        tmpPtr = &MB.M0_Coef;
        DataEEWrite(tmpPtr[i*2], FLASH_ADC_COEF_ADDR + i*2); 
        DataEEWrite(tmpPtr[i*2 + 1], FLASH_ADC_COEF_ADDR + i*2 + 1);
        
        tmpPtr = &MB.M0_RMS_sign_threshold;        
        DataEEWrite(tmpPtr[i*2], FLASH_SIGN_THRESH_ADDR + i*2); 
        DataEEWrite(tmpPtr[i*2 + 1], FLASH_SIGN_THRESH_ADDR + i*2 + 1);        
    }
    
    DataEEWrite(MB.N, FLASH_N_ADDR);
    DataEEWrite(MB.D_OutInit, FLASH_DOUT_INIT_ADDR);
    
    tmpPtr = &MB.DI0_ImpFreqCoef;
    for(i = 0; i < 4; ++i) {            
        DataEEWrite(tmpPtr[i*2], FLASH_DI_IMP_FREQ_COEF_ADDR + i*2); 
        DataEEWrite(tmpPtr[i*2 + 1], FLASH_DI_IMP_FREQ_COEF_ADDR + i*2 + 1);
    }
    
    DataEEWrite(MB.disk_imp_count, FLASH_DISK_IMP_COUNT_ADDR);
    
    DataEEWrite(MB.Ind_Delay, FLASH_IND_DELAY_ADDR);
    
    DataEEWrite(MB.PROF_CHANGE_SOURCE, FLASH_PROF_CHANGE_SOURCE_ADDR);
        
    unsigned int *indProfilesPtr = &MB.profile;
    DataEEWrite(MB.profile, FLASH_PROFILES_ADDR);
    for(i = 1; i < 13; ++i) {
        DataEEWrite(indProfilesPtr[i], FLASH_PROFILES_ADDR + i);
    }
    
    tmpPtr = &MB.P1_coef;
    for(i = 0; i < 3; ++i){
        DataEEWrite(tmpPtr[i*2], FLASH_POWER_COEF_ADDR + i*2); 
        DataEEWrite(tmpPtr[i*2 + 1], FLASH_POWER_COEF_ADDR + i*2 + 1);
    }
    
    DataEEWrite(MB.LCD_rows, FLASH_LCD_ROWS_ADDR);
    DataEEWrite(MB.LCD_cols, FLASH_LCD_COLS_ADDR);
    
    tmpPtr = &MB.LCD_FORMAT_STRING_1;
    for(i = 0; i < FORMAT_STRING_BUF_SIZE * 2; ++i) {
        DataEEWrite(tmpPtr[i],FLASH_LCD_FORMAT_ADDR + i);
    }
}

void loadParams() {
    DataEEInit();
    
    unsigned int isFirstStart = DataEERead(FLASH_FIRST_START_FLAG_ADDR);
    
    int i;
    int *offsetPtr = &MB.M0_OFFSET;
    float *coefPtr = &MB.M0_Coef;
    float * RMSSignThreshPtr = &MB.M0_RMS_sign_threshold;
    unsigned int *tmpPtr;
    
    if(isFirstStart) {
        MB.ADDRESS = DEFAULT_MODBUS_ADDRESS;
        DataEEWrite(MB.ADDRESS, FLASH_MB_ADDRESS_ADDR); 
        
        MB.BAUD_RATE = DEFAULT_UART_BAUDRATE;
        tmpPtr = &MB.BAUD_RATE;
        DataEEWrite(tmpPtr[0], FLASH_BAUD_RATE_ADDR); 
        DataEEWrite(tmpPtr[1], FLASH_BAUD_RATE_ADDR + 1); 
        
        for(i = 0; i < 7; ++i) {
            offsetPtr[i] = 0;
            DataEEWrite(offsetPtr[i], FLASH_ADC_OFFSET_ADDR + i); 
            coefPtr[i] = 1.0;
            
            tmpPtr = &MB.M0_Coef;
            DataEEWrite(tmpPtr[i*2], FLASH_ADC_COEF_ADDR + i*2); 
            DataEEWrite(tmpPtr[i*2 + 1], FLASH_ADC_COEF_ADDR + i*2 + 1);
            
            tmpPtr = &MB.M0_RMS_sign_threshold; 
            RMSSignThreshPtr[i] = 0.6f;
            DataEEWrite(tmpPtr[i*2], FLASH_SIGN_THRESH_ADDR + i*2); 
            DataEEWrite(tmpPtr[i*2 + 1], FLASH_SIGN_THRESH_ADDR + i*2 + 1);
        }
        
        MB.N = 200;
        DataEEWrite(MB.N, FLASH_N_ADDR);
        
        MB.D_OutInit = 0;
        DataEEWrite(MB.D_OutInit, FLASH_DOUT_INIT_ADDR);
        
        float *DICoefPtr = &MB.DI0_ImpFreqCoef;
        for(i = 0; i < 4; ++i) {
            DICoefPtr[i] = 1.0;
            
            tmpPtr = &MB.DI0_ImpFreqCoef;
            DataEEWrite(tmpPtr[i*2], FLASH_DI_IMP_FREQ_COEF_ADDR + i*2); 
            DataEEWrite(tmpPtr[i*2 + 1], FLASH_DI_IMP_FREQ_COEF_ADDR + i*2 + 1);
        }
        
        MB.disk_imp_count = 0;
        DataEEWrite(MB.disk_imp_count, FLASH_DISK_IMP_COUNT_ADDR);
        
        MB.Ind_Delay = 25;
        DataEEWrite(MB.Ind_Delay, FLASH_IND_DELAY_ADDR);
        
        MB.PROF_CHANGE_SOURCE = 0;
        DataEEWrite(MB.PROF_CHANGE_SOURCE, FLASH_PROF_CHANGE_SOURCE_ADDR);
         
        unsigned int *indProfilesPtr = &MB.profile;
        MB.profile = 1;
        DataEEWrite(MB.profile, FLASH_PROFILES_ADDR);
        for(i = 1; i < 13; ++i) {
            indProfilesPtr[i] = DEFAULT_IND_PROFILE;
            DataEEWrite(indProfilesPtr[i], FLASH_PROFILES_ADDR + i);
        }
        
        float *powerCoefsPtr = &MB.P1_coef;
        tmpPtr = &MB.P1_coef;
        for(i = 0; i < 3; ++i){
            powerCoefsPtr[i] = 1.0f;
            
            DataEEWrite(tmpPtr[i*2], FLASH_POWER_COEF_ADDR + i*2); 
            DataEEWrite(tmpPtr[i*2 + 1], FLASH_POWER_COEF_ADDR + i*2 + 1);
        }
        
        MB.LCD_rows = 0;
        MB.LCD_cols = 0;
        DataEEWrite(MB.LCD_rows, FLASH_LCD_ROWS_ADDR);
        DataEEWrite(MB.LCD_cols, FLASH_LCD_COLS_ADDR);
        
        DataEEWrite(0, FLASH_FIRST_START_FLAG_ADDR);
    } else {
        MB.ADDRESS = DataEERead(FLASH_MB_ADDRESS_ADDR);
        
        tmpPtr = &MB.BAUD_RATE;
        tmpPtr[0] = DataEERead(FLASH_BAUD_RATE_ADDR);
        tmpPtr[1] = DataEERead(FLASH_BAUD_RATE_ADDR + 1);
        
        for(i = 0; i < 7; ++i) {
           offsetPtr[i] = DataEERead(FLASH_ADC_OFFSET_ADDR + i);

           tmpPtr = &MB.M0_Coef;
           tmpPtr[i*2] = DataEERead(FLASH_ADC_COEF_ADDR + i*2);
           tmpPtr[i*2 + 1] = DataEERead(FLASH_ADC_COEF_ADDR + i*2 + 1);
           
           tmpPtr = &MB.M0_RMS_sign_threshold;
           tmpPtr[i*2] = DataEERead(FLASH_SIGN_THRESH_ADDR + i*2);
           tmpPtr[i*2 + 1] = DataEERead(FLASH_SIGN_THRESH_ADDR + i*2 + 1);
        }
        
        MB.N = DataEERead(FLASH_N_ADDR);
        MB.D_OutInit = DataEERead(FLASH_DOUT_INIT_ADDR);
        
        tmpPtr = &MB.DI0_ImpFreqCoef;
        for(i = 0; i < 4; ++i) {
           tmpPtr[i*2] = DataEERead(FLASH_DI_IMP_FREQ_COEF_ADDR + i*2);
           tmpPtr[i*2 + 1] = DataEERead(FLASH_DI_IMP_FREQ_COEF_ADDR + i*2 + 1);
        }
        
        MB.disk_imp_count = DataEERead(FLASH_DISK_IMP_COUNT_ADDR);
        
        MB.Ind_Delay = DataEERead(FLASH_IND_DELAY_ADDR);
        
        MB.PROF_CHANGE_SOURCE  = DataEERead(FLASH_PROF_CHANGE_SOURCE_ADDR);
        
        unsigned int *indProfilesPtr = &MB.profile;
        for(i = 0; i < 13; ++i) {
            indProfilesPtr[i] = DataEERead(FLASH_PROFILES_ADDR + i);
        }
        
        tmpPtr = &MB.P1_coef;
        for(i = 0; i < 3; ++i){
            tmpPtr[i*2] = DataEERead(FLASH_POWER_COEF_ADDR + i*2); 
            tmpPtr[i*2 + 1] = DataEERead(FLASH_POWER_COEF_ADDR + i*2 + 1);
        }
        
        MB.LCD_rows = DataEERead(FLASH_LCD_ROWS_ADDR);
        MB.LCD_cols = DataEERead(FLASH_LCD_COLS_ADDR);
        
        tmpPtr = &MB.LCD_FORMAT_STRING_1;
        for(i = 0; i < FORMAT_STRING_BUF_SIZE * 2; ++i) {
            tmpPtr[i] = DataEERead(FLASH_LCD_FORMAT_ADDR + i);
        }
    }
}