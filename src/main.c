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


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

bool crcCorrect(uint8_t* buf, uint8_t len) {
    unsigned int crc;
    if (len < 3) {
        return 0;
    }
    crc = Crc16(buf, len - 2);
    uint8_t CRC_16_Lo = crc & 0xFF;
    uint8_t CRC_16_Hi = (crc & 0xFF00) >> 8;
    return ((CRC_16_Lo == buf[len - 2]) && (CRC_16_Hi == buf[len-1]));
}

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
       
    MB.ADDRESS = DEFAULT_MODBUS_ADDRESS;
    
        // Main cycle
    while (1) {
       
        // Perform Modbus protocol processing
        Modbus_RTU();
                   
        // Update display
        display_update(1);
        
        if(MB.FLASH_WRITE == 1) {
           MB.FLASH_WRITE = 0;
        }
        
        if(MB.RESET == 1) {
            MB.RESET = 0;
        }
                            
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}
