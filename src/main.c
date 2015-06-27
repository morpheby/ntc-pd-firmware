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


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

#define RAM_START_ADDRESS               0x900


int imp_kol_temp = 0;
int PARAM_SET;
int ind_off = 0x07;

// Flash storage for permanent modbus registers
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_K[7] = {1., 1., 1., 1., 1., 1., 1.};
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_OFFSET[7] = {0, 0, 0, 0, 0, 0, 0};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_PosK0 = .125;

int PROF=1;
char MENU_LEVEL = 0;

int _temp_BAUD=0;

int *RamData;

// Input filtering
static long int arr[7]= {0, 0, 0, 0, 0, 0, 0};
static unsigned int init[7] = {0, 0, 0, 0, 0, 0, 0};

#define FILTER_COUNT 1000

// Filters ADC input data
unsigned int filter (unsigned int  value, uint8_t channel_num) {
    if (init[channel_num] < FILTER_COUNT) {
        arr[channel_num] += value;
        ++init[channel_num];
        return 0;
    } else {
        arr[channel_num] += (-arr[channel_num]/FILTER_COUNT + value);
        return arr[channel_num]/FILTER_COUNT;
    }
}

void __attribute__((interrupt,no_auto_psv)) _QEIInterrupt() {
     IFS3bits.QEIIF = 0;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt() {
   MB.A0=filter(ADC1BUF0,0);
   MB.A1=filter(ADC1BUF1,1);
   MB.A2=filter(ADC1BUF2,2);
   MB.A3=filter(ADC1BUF3,3);
   MB.A4=filter(ADC1BUF4,4);
   MB.A5=filter(ADC1BUF5,5);
   MB.A6=filter(ADC1BUF6,6);
   
   MB.ADC0=((int)MB.A0 - MB.OFS_ADC0)*MB.K0;   //AN6
   MB.ADC1=((int)MB.A1 - MB.OFS_ADC1)*MB.K1;   //AN3
   MB.ADC2=((int)MB.A2 - MB.OFS_ADC2)*MB.K2;   //AN4
   MB.ADC3=((int)MB.A3 - MB.OFS_ADC3)*MB.K3;   //AN5
   MB.ADC4=((int)MB.A4 - MB.OFS_ADC4)*MB.K4;   //AN0
   MB.ADC5=((int)MB.A5 - MB.OFS_ADC5)*MB.K5;   //AN1
   MB.ADC6=((int)MB.A6 - MB.OFS_ADC6)*MB.K6;   //AN2
 
   IFS0bits.AD1IF=0;
}


MAIN_DECL_LOOP_FN();

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main() {
    
    /* Initialize reset parser */
    system_init();

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
    
    ADC_Init(1);
    discrete_init();
    
    MB.FLASH_WR = 0;
    
    MB.Ind_Delay = 100; 
    MB.N = 4000;
    MB.BRG_VAL = 19200;
   
    uint16_t i = 0;
    float *adcCoeffPtr = &MB.K0;
    int *adcOffsetPtr = &MB.OFS_ADC0;
    uint16_t *tmpPtr;
    RamData = &(MB.BRG_VAL);

    // Init permanently stored values for coeffs
    for (i = 0; i < 7; ++i) {
        adcCoeffPtr[i]  = flash_data_buf_K[i];
        adcOffsetPtr[i] = flash_data_buf_OFFSET[i];
    }
    MB.PositionK0 = flash_data_buf_PosK0;
    
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
            flash_write();
            system_reset();
            MB.FLASH_WR = 0;
        }
        
        display_update();
        
        // Perform Modbus protocol processing
        Modbus_RTU();
        
        // Internal Modbus function for framing
        RS_Update();
        
        // Update discrete outputs and resample discrete inputs
        discrete_update();
        
        // Call application-end function
        MAIN_CALL_LOOP_FN();
        
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}

