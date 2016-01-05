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
#include "DS1820.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_MB_ADDRESS = DEFAULT_MODBUS_ADDRESS;
int _FLASH_STORE _FLASH_ACCESS flash_data_buf_ADC_OFFSET[7] = {0,0,0,0,0,0,0};
float _FLASH_STORE _FLASH_ACCESS flash_data_buf_ADC_COEF[7] = {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
unsigned int _FLASH_STORE _FLASH_ACCESS flash_data_buf_N = 200;

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
       
    MB.ADDRESS = flash_data_buf_MB_ADDRESS;
    setModbusAddress(MB.ADDRESS);
    
    uint8_t i;
    int *offsetPtr = &MB.M0_OFFSET;
    float *coefPtr = &MB.M0_Coef;
    for(i = 0; i < 7; ++i) {
        offsetPtr[i] = flash_data_buf_ADC_OFFSET[i];
        coefPtr[i] = flash_data_buf_ADC_COEF[i];
    }
    
    MB.N = flash_data_buf_N;
    
    app_init();
    uint16_t *tmpPtr;
    // Main cycle
    while (1) {
       
        // Perform Modbus protocol processing
        Modbus_RTU();
        
        perform_data_operations();
                   
        // Update display
        display_update(1);
        
        if(MB.FLASH_WRITE == 1) {
            if(MB.ADDRESS != flash_data_buf_MB_ADDRESS) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_MB_ADDRESS), FLASH_GETOFFSET(&flash_data_buf_MB_ADDRESS),
                        MB.ADDRESS);                 
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
            }
            if(MB.N != flash_data_buf_N) {
                // Only perform if the data has changed, spare memory
                flash_set(FLASH_GETPAGE(&flash_data_buf_N), FLASH_GETOFFSET(&flash_data_buf_N), MB.N);                 
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
