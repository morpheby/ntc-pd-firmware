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

static uint8_t _rx_buf[200];
static uint8_t _rx_len = 0;
static uint8_t _tx_buf[7] = {0x01, 0x03, 0x02, 0x00, 0x00, 0xB8, 0x44};
static uint8_t _tx_len = 0;
static uint8_t _tx_pos = 0;
static uint32_t _frame_start_time = 0;

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
    
        // Main cycle
    while (1) {
       
        // Perform Modbus protocol processing
        //Modbus_RTU();
        
        // Internal Modbus function for framing
        //RS_Update();
           
        // Update display
        display_update(1);
        if(_rx_len > 0 && _frame_start_time + 200 <= timing_get_time_msecs()) {
            IEC0bits.U1RXIE=0;		// Rx Disable
            
            _tx_pos = 0;
            _tx_len = 7;
            U1TXREG = _tx_buf[0];
            
            _rx_len = 0;
            IEC0bits.U1RXIE=1;
        }
                    
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}

void _ISR_NOPSV _U1TXInterrupt() {
   if (++_tx_pos < _tx_len)
	{
		U1TXREG = _tx_buf[_tx_pos];
	}
    IFS0bits.U1TXIF = 0;
}

void _ISR_NOPSV _U1RXInterrupt() 
{
    if(!U1STAbits.URXDA) {
        return;
    }
    uint8_t recieved = U1RXREG;
    
    if(_rx_len == 0) {
        _frame_start_time = timing_get_time_msecs();
    }
    _rx_len ++;
    
   
    // Clear interrupt flag
    IFS0bits.U1RXIF = 0;
}