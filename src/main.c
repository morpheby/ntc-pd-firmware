/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "oscillators.h"
#include "uart_base.h"
#include "flash_store.h"
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


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
#define DEFAULT_IND_PROFILE 0xFF00

unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_MB_ADDRESS = DEFAULT_MODBUS_ADDRESS;
unsigned long int _FLASH_STORE _FLASH_ACCESS flash_data_buf_BAUD_RATE = DEFAULT_UART_BAUDRATE;
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_ADC_OFFSET[7] = {0,0,0,0,0,0,0};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_ADC_COEF[7] = {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_N = 200;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_D_OUT_INIT = 0;
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_DI_FREQ_COEF[4] = {1.0f,1.0f,1.0f,1.0f};
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_DISK_IMP_COUNT = 0;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_IND_DELAY = 25;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_PROF_CHANGE_SOURCE = 0;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_IND_PROFILES[13] = {1, //current profile number
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE,
                                                                     DEFAULT_IND_PROFILE, 
                                                                     DEFAULT_IND_PROFILE};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_ADC_RMS_SIGN_THRESHOLDS[7] = {0.6f,0.6f,0.6f,0.6f,0.6f,0.6f,0.6f};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_POWER_COEFS[3] = {1.0f,1.0f,1.0f};

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
    
    /* Initialize RTSP */
    flash_init();
    
    /* Initialize modbus registers */
    modbus_regs_init();
    
    /* Initialize UART */
    MB.BAUD_RATE = flash_data_buf_BAUD_RATE;
    uart_init();
        
    /* Initialize system timing */
    timing_init();
            
    ADC_Init(1);
    discrete_init();
       
    MB.ADDRESS = flash_data_buf_MB_ADDRESS;
    setModbusAddress(MB.ADDRESS);
    float periode = 10.0/MB.BAUD_RATE*3.5;
    set_mb_silence_timer_periode((unsigned int)periode+1);
    
    uint8_t i;
    int *offsetPtr = &MB.M0_OFFSET;
    float *coefPtr = &MB.M0_Coef;
    float *rmsSignThresholdPtr = &MB.M0_RMS_sign_threshold;
    for(i = 0; i < 7; ++i) {
        offsetPtr[i] = flash_data_buf_ADC_OFFSET[i];
        coefPtr[i] = flash_data_buf_ADC_COEF[i];
        rmsSignThresholdPtr[i] = flash_data_buf_ADC_RMS_SIGN_THRESHOLDS[i];
    }
    
    MB.N = flash_data_buf_N;
    MB.D_OutInit = flash_data_buf_D_OUT_INIT;
    MB.D_Out = MB.D_OutInit;
    MB.disk_imp_count = flash_data_buf_DISK_IMP_COUNT;
    MB.Ind_Delay = flash_data_buf_IND_DELAY;
    MB.PROF_CHANGE_SOURCE = flash_data_buf_PROF_CHANGE_SOURCE;
    
    float *DICoefPtr = &MB.DI0_ImpFreqCoef;
    for(i = 0; i < 4; ++i) {
        DICoefPtr[i] = flash_data_buf_DI_FREQ_COEF[i];
    }
    unsigned int *indProfilesPtr = &MB.profile;
    for(i = 0; i < 13; ++i) {
        indProfilesPtr[i] = flash_data_buf_IND_PROFILES[i];
    }
    float *powerCoefsPtr = &MB.P1_coef;
    for(i = 0; i < 3; ++i){
        powerCoefsPtr[i] = flash_data_buf_POWER_COEFS[i];
    }
    
    app_init();
    uint16_t *tmpPtr;
    // Main cycle
    while (1) {              
        ind_showValues(); 
        display_update(0);
        discrete_update();
        perform_data_operations();
                                   
        if(MB.FLASH_WRITE == 1) {
            if(MB.ADDRESS != flash_data_buf_MB_ADDRESS) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_MB_ADDRESS), FLASH_GETOFFSET(&flash_data_buf_MB_ADDRESS),
                        MB.ADDRESS);                 
            }
            if(MB.BAUD_RATE != flash_data_buf_BAUD_RATE) {
                tmpPtr = &MB.BAUD_RATE;
                flash_set(FLASH_GETPAGE(&flash_data_buf_BAUD_RATE), FLASH_GETOFFSET(&flash_data_buf_BAUD_RATE),
                        tmpPtr[0]);
                flash_set(FLASH_GETPAGE(&flash_data_buf_BAUD_RATE), FLASH_GETOFFSET(&flash_data_buf_BAUD_RATE)+2,
                        tmpPtr[1]);                 
            }
            for(i = 0; i < 7; ++i) {
                if(offsetPtr[i] != flash_data_buf_ADC_OFFSET[i])
                {
                    // Only perform if the data has changed, spare memory
                    flash_set(FLASH_GETPAGE(flash_data_buf_ADC_OFFSET), FLASH_GETAOFFSET(flash_data_buf_ADC_OFFSET, i),
                            offsetPtr[i]);
                }  
                if(coefPtr[i] != flash_data_buf_ADC_COEF[i])
                {
                    tmpPtr = coefPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_ADC_COEF), FLASH_GETAOFFSET(flash_data_buf_ADC_COEF, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_ADC_COEF), FLASH_GETAOFFSET(flash_data_buf_ADC_COEF, i)+2,
                            tmpPtr[1]);
                }    
                if(rmsSignThresholdPtr[i] != flash_data_buf_ADC_RMS_SIGN_THRESHOLDS[i])
                {
                    tmpPtr = rmsSignThresholdPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_ADC_RMS_SIGN_THRESHOLDS), FLASH_GETAOFFSET(flash_data_buf_ADC_RMS_SIGN_THRESHOLDS, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_ADC_RMS_SIGN_THRESHOLDS), FLASH_GETAOFFSET(flash_data_buf_ADC_RMS_SIGN_THRESHOLDS, i)+2,
                            tmpPtr[1]);
                }                   
            }
            if(MB.N != flash_data_buf_N) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_N), FLASH_GETOFFSET(&flash_data_buf_N), MB.N);                 
            }
            if(MB.D_OutInit != flash_data_buf_D_OUT_INIT) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_D_OUT_INIT), FLASH_GETOFFSET(&flash_data_buf_D_OUT_INIT), MB.D_OutInit);                 
            }
            for(i = 0; i < 4; ++i) { 
                if(DICoefPtr[i] != flash_data_buf_DI_FREQ_COEF[i])
                {
                    tmpPtr = DICoefPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_DI_FREQ_COEF), FLASH_GETAOFFSET(flash_data_buf_DI_FREQ_COEF, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_DI_FREQ_COEF), FLASH_GETAOFFSET(flash_data_buf_DI_FREQ_COEF, i)+2,
                            tmpPtr[1]);
                }                      
            }
            if(MB.disk_imp_count != flash_data_buf_DISK_IMP_COUNT) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_DISK_IMP_COUNT), FLASH_GETOFFSET(&flash_data_buf_DISK_IMP_COUNT), MB.disk_imp_count);                 
            }
            if(MB.Ind_Delay != flash_data_buf_IND_DELAY) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_IND_DELAY), FLASH_GETOFFSET(&flash_data_buf_IND_DELAY), MB.Ind_Delay);                 
            }
            if(MB.PROF_CHANGE_SOURCE != flash_data_buf_PROF_CHANGE_SOURCE) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_PROF_CHANGE_SOURCE), FLASH_GETOFFSET(&flash_data_buf_PROF_CHANGE_SOURCE), MB.PROF_CHANGE_SOURCE);                 
            }
            for(i = 0; i < 13; ++i) {
                if(flash_data_buf_IND_PROFILES[i] != indProfilesPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    flash_set(FLASH_GETPAGE(flash_data_buf_IND_PROFILES), FLASH_GETAOFFSET(flash_data_buf_IND_PROFILES, i),
                            indProfilesPtr[i]);                    
                }
            }
            for(i = 0; i < 3; ++i) { 
                if(powerCoefsPtr[i] != flash_data_buf_POWER_COEFS[i])
                {
                    tmpPtr = powerCoefsPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_POWER_COEFS), FLASH_GETAOFFSET(flash_data_buf_POWER_COEFS, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_POWER_COEFS), FLASH_GETAOFFSET(flash_data_buf_POWER_COEFS, i)+2,
                            tmpPtr[1]);
                }                      
            }
            flash_write();
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
