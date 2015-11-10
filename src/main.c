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
#include "menu-base.h"

#include "modbus.h"
#include "ADC.h"
#include "math.h"
#include "D_I_O.h"
#include "modbus_registers.h"
#include "filter.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

#define RAM_START_ADDRESS               0x900


int imp_kol_temp = 0;
int PARAM_SET;
int ind_off = 0x07;

#define DEFAULT_IND_PROFILE 0xFF00

// Flash storage for permanent modbus registers
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_K[7] = {1., 1., 1., 1., 1., 1., 1.};
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_OFFSET[7] = {0, 0, 0, 0, 0, 0, 0};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_PosK0 = .125;
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_IND_PROFILES[13] = {1, //current profile number
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
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_PROF_CHANGE_SOURCE = 0;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_N = 4000;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_Ind_Delay = 100;
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_D_Out_Init = 0;
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_P1_coef = 0.99;
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_P2_coef = 0.99;
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_P3_coef = 0.99;

int PROF=1;
char MENU_LEVEL = 0;

int _temp_BAUD=0;

int *RamData;


MAIN_DECL_LOOP_FN();

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
    
    /* Initialize menu-base */
    disp_config();
    
    /* Initialize Change-notification inputs */
    cni_init();
    
    /* Initialize oscillator switcher */
    osc_init();

    // Select Internal FRC with PLL
    osc_select(oscBoosted);
    
    /* Initialize UART */
    uart_init();
    
    /* Initialize RTSP */
    flash_init();
    
    /* Initialize system timing */
    timing_init();
    
    /* Initialize modbus registers */
    modbus_regs_init();
    
    /* Initialize filters */
    filter_init();
    
    ADC_Init(1);
    discrete_init();
    
    MB.FLASH_WR = 0;
    
    MB.Ind_Delay = flash_data_buf_Ind_Delay; 
    MB.N = flash_data_buf_N;
    MB.D_Out_Init = flash_data_buf_D_Out_Init;
    MB.D_Out = MB.D_Out_Init;
    MB.P1_coef = flash_data_buf_P1_coef;
    MB.P2_coef = flash_data_buf_P2_coef;
    MB.P3_coef = flash_data_buf_P3_coef;
    MB.BRG_VAL = 19200;
   
    uint16_t i = 0;
    float *adcCoeffPtr = &MB.K0;
    int *adcOffsetPtr = &MB.OFS_ADC0;
    int *indProfilesPtr = &MB.profile;
    uint16_t *tmpPtr;
    RamData = &(MB.BRG_VAL);

    // Init permanently stored values for coeffs
    for (i = 0; i < 7; ++i) {
        adcCoeffPtr[i]  = flash_data_buf_K[i];
        adcOffsetPtr[i] = flash_data_buf_OFFSET[i];
    }
    MB.PositionK0 = flash_data_buf_PosK0;
    
    //Init permamnently stored values for ind profiles
    for(i = 0; i < 13; ++i) {
        indProfilesPtr[i] = flash_data_buf_IND_PROFILES[i];
    }
    MB.PROF_CHANGE_SOURCE = flash_data_buf_PROF_CHANGE_SOURCE;
    
    // Initialize application-specific module
    app_init();
    
    // Main cycle
    while (1) {
        // Perform RTSP, if externally requested
        if (MB.FLASH_WR == 1) {
            for (i = 0; i < 7; i++) {
                if (flash_data_buf_K[i] != adcCoeffPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    tmpPtr = adcCoeffPtr+i;
                    flash_set(FLASH_GETPAGE(flash_data_buf_K), FLASH_GETAOFFSET(flash_data_buf_K, i),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(flash_data_buf_K), FLASH_GETAOFFSET(flash_data_buf_K, i)+2,
                            tmpPtr[1]);
                }
                if (flash_data_buf_OFFSET[i] != adcOffsetPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    flash_set(FLASH_GETPAGE(flash_data_buf_OFFSET), FLASH_GETAOFFSET(flash_data_buf_OFFSET, i),
                            adcOffsetPtr[i]);
                }
                if (flash_data_buf_PosK0 != MB.PositionK0) {
                    tmpPtr = &MB.PositionK0;
                    flash_set(FLASH_GETPAGE(&flash_data_buf_PosK0), FLASH_GETOFFSET(&flash_data_buf_PosK0),
                            tmpPtr[0]);
                    flash_set(FLASH_GETPAGE(&flash_data_buf_PosK0), FLASH_GETOFFSET(&flash_data_buf_PosK0)+2,
                            tmpPtr[1]);
                }
            }
            for(i = 0; i < 13; ++i) {
                if(flash_data_buf_IND_PROFILES[i] != indProfilesPtr[i]) {
                    // Only perform if the data has changed, spare memory
                    flash_set(FLASH_GETPAGE(flash_data_buf_IND_PROFILES), FLASH_GETAOFFSET(flash_data_buf_IND_PROFILES, i),
                            indProfilesPtr[i]);                    
                }
            }
            if(MB.PROF_CHANGE_SOURCE != flash_data_buf_PROF_CHANGE_SOURCE) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_PROF_CHANGE_SOURCE), FLASH_GETOFFSET(&flash_data_buf_PROF_CHANGE_SOURCE),
                        MB.PROF_CHANGE_SOURCE);                 
            }
            if(MB.N != flash_data_buf_N) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_N), FLASH_GETOFFSET(&flash_data_buf_N),
                        MB.N);                 
            }
            if(MB.D_Out_Init != flash_data_buf_D_Out_Init) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_D_Out_Init), FLASH_GETOFFSET(&flash_data_buf_D_Out_Init),
                        MB.D_Out_Init);                 
            }
            if(MB.Ind_Delay != flash_data_buf_Ind_Delay) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_Ind_Delay), FLASH_GETOFFSET(&flash_data_buf_Ind_Delay),
                        MB.Ind_Delay);                 
            }
            if(MB.P1_coef != flash_data_buf_P1_coef) {
                tmpPtr = &MB.P1_coef;
                flash_set(FLASH_GETPAGE(&flash_data_buf_P1_coef), FLASH_GETOFFSET(&flash_data_buf_P1_coef),
                        tmpPtr[0]);
                flash_set(FLASH_GETPAGE(&flash_data_buf_P1_coef), FLASH_GETOFFSET(&flash_data_buf_P1_coef)+2,
                        tmpPtr[1]);             
            }
            if(MB.P2_coef != flash_data_buf_P2_coef) {
                tmpPtr = &MB.P2_coef;
                flash_set(FLASH_GETPAGE(&flash_data_buf_P2_coef), FLASH_GETOFFSET(&flash_data_buf_P2_coef),
                        tmpPtr[0]);
                flash_set(FLASH_GETPAGE(&flash_data_buf_P2_coef), FLASH_GETOFFSET(&flash_data_buf_P2_coef)+2,
                        tmpPtr[1]);                 
            }
            if(MB.P3_coef != flash_data_buf_P3_coef) {
                tmpPtr = &MB.P3_coef;
                flash_set(FLASH_GETPAGE(&flash_data_buf_P3_coef), FLASH_GETOFFSET(&flash_data_buf_P3_coef),
                        tmpPtr[0]);
                flash_set(FLASH_GETPAGE(&flash_data_buf_P3_coef), FLASH_GETOFFSET(&flash_data_buf_P3_coef)+2,
                        tmpPtr[1]);              
            }
            flash_write();
            system_reset();
            MB.FLASH_WR = 0;
        }
        
        // Perform Modbus protocol processing
        Modbus_RTU();
        
        // Internal Modbus function for framing
        RS_Update();
        
        // Update discrete outputs and resample discrete inputs
        discrete_update();
        
        // Call application-end function
        MAIN_CALL_LOOP_FN();
        
        // Update display
        display_update(1);
        
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}

