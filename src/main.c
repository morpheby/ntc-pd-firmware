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
static uint8_t _tx_buf[200] = {0x01, 0x03, 0x02, 0x00, 0x00, 0xB8, 0x44};
static uint8_t _tx_len = 0;
static uint8_t _tx_pos = 0;
static uint32_t _frame_start_time = 0;

#define MB_STATE_READY 0
#define MB_STATE_CMD_WAIT 1
#define MB_STATE_DATA_WAIT 2

static uint8_t _mb_state = MB_STATE_READY;

#define READ_HOLDING_REGISTERS		0x03
#define PRESET_SINGLE_REGISTER		0x06
#define PRESET_MULTIPLE_REGISTERS	0x10

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
    
    MB.BRG_VAL = 19200;
    
        // Main cycle
    while (1) {
       
        // Perform Modbus protocol processing
        //Modbus_RTU();
        
        // Internal Modbus function for framing
        //RS_Update();
           
        // Update display
        display_update(1);
        if(_mb_state != MB_STATE_READY && _frame_start_time + 40 <= timing_get_time_msecs()) {
            IEC0bits.U1RXIE=0;		// Rx Disable
            if(crcCorrect(_rx_buf, _rx_len)) {
                uint16_t* mbDataPtr = &MB.BRG_VAL;
                unsigned int start = (_rx_buf[2] << 8) | _rx_buf[3];
                unsigned int count = (_rx_buf[4] << 8) | _rx_buf[5];
                uint8_t regIndex;
                switch(_rx_buf[1]) {
                    case READ_HOLDING_REGISTERS:
                    {
                        IEC0bits.U1TXIE=0;                       
                        _tx_buf[0] = MODBUS_ADDRESS;
                        _tx_buf[1] = READ_HOLDING_REGISTERS;
                        _tx_buf[2] = count*2;
                        
                        for(regIndex = 0; regIndex < count; ++regIndex) {
                            uint16_t regData = mbDataPtr[start+regIndex];
                            _tx_buf[3 + regIndex*2] = regData>>8;
                            _tx_buf[3 + regIndex*2 + 1] = regData & 0x00FF;
                        }
                        uint8_t len = 2*count + 5;
                        uint16_t crc = Crc16(_tx_buf, len - 2);
                        uint8_t CRC_16_Lo = crc & 0xFF;
                        uint8_t CRC_16_Hi = (crc & 0xFF00) >> 8;
                        
                        _tx_buf[len-2] = CRC_16_Lo;
                        _tx_buf[len-1] = CRC_16_Hi;
                        _tx_pos = 0;
                        _tx_len = len;
                        IEC0bits.U1TXIE=1;
                        U1TXREG = _tx_buf[0];          
                        break;
                    }
                    case PRESET_SINGLE_REGISTER:
                    {
                        IEC0bits.U1TXIE=0; 
                        mbDataPtr[start]=((_rx_buf[4] << 8)) | _rx_buf[5];                
                        _tx_buf[0] = MODBUS_ADDRESS;
                        _tx_buf[1] = PRESET_MULTIPLE_REGISTERS;
                        _tx_buf[2] = _rx_buf[4];
                        _tx_buf[3] = _rx_buf[5];
                        _tx_buf[4]=count >> 8;
                        _tx_buf[5]=count & 0x00FF;
                        
                        uint8_t len = 8;
                        uint16_t crc = Crc16(_tx_buf, len - 2);
                        uint8_t CRC_16_Lo = crc & 0xFF;
                        uint8_t CRC_16_Hi = (crc & 0xFF00) >> 8;
                        
                        _tx_buf[len-2] = CRC_16_Lo;
                        _tx_buf[len-1] = CRC_16_Hi;
                        _tx_pos = 0;
                        _tx_len = len;
                        IEC0bits.U1TXIE=1;
                        U1TXREG = _tx_buf[0]; 
                        break;
                    }
                    case PRESET_MULTIPLE_REGISTERS:
                    {
                        IEC0bits.U1TXIE=0; 
                        for(regIndex = 0; regIndex < count; ++regIndex) {
                            mbDataPtr[start + regIndex] = ((_rx_buf[regIndex*2+7] << 8)) | _rx_buf[regIndex*2+8];
                        }                      
                        _tx_buf[0] = MODBUS_ADDRESS;
                        _tx_buf[1] = PRESET_MULTIPLE_REGISTERS;
                        _tx_buf[2] = start >> 8;
                        _tx_buf[3] = start & 0x00FF;
                        _tx_buf[4]=count >> 8;
                        _tx_buf[5]=count & 0x00FF;
                        
                        uint8_t len = 8;
                        uint16_t crc = Crc16(_tx_buf, len - 2);
                        uint8_t CRC_16_Lo = crc & 0xFF;
                        uint8_t CRC_16_Hi = (crc & 0xFF00) >> 8;
                        
                        _tx_buf[len-2] = CRC_16_Lo;
                        _tx_buf[len-1] = CRC_16_Hi;
                        _tx_pos = 0;
                        _tx_len = len;
                        IEC0bits.U1TXIE=1;
                        U1TXREG = _tx_buf[0];  
                        break;
                    }
                }
            }
            
            _rx_len = 0;
            _mb_state = MB_STATE_READY;
            IEC0bits.U1RXIE=1;
        }
                    
        // Clear WDT flag to indicate normal operation
        wdt_clr();
    }
}

void _ISR_NOPSV _U1TXInterrupt() {
    if (++_tx_pos < _tx_len && !U1STAbits.UTXBF)
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
    
    switch(_mb_state) {
        case (MB_STATE_READY):
        {
            if(recieved == MODBUS_ADDRESS) {
                _frame_start_time = timing_get_time_msecs();
                _rx_buf[_rx_len] = recieved;
                _rx_len ++;
                _mb_state = MB_STATE_CMD_WAIT;
            }
            break;
        }
        case (MB_STATE_CMD_WAIT):
        {
            switch (recieved) {
                case READ_HOLDING_REGISTERS:
                {
                    _mb_state = MB_STATE_DATA_WAIT;
                    break;
                }
                case PRESET_SINGLE_REGISTER:
                {
                    _mb_state = MB_STATE_DATA_WAIT;
                    break;
                }
                case PRESET_MULTIPLE_REGISTERS:
                {
                    _mb_state = MB_STATE_DATA_WAIT;
                    break;
                }
                default:
                {
                    _mb_state = MB_STATE_READY;
                }
            }
            _rx_buf[_rx_len] = recieved;
            _rx_len++;
            break;
        }
        case (MB_STATE_DATA_WAIT):
        {
            _rx_buf[_rx_len] = recieved;
            _rx_len++;
            break;
        }
    }
    
  /*  if(_rx_len == 0 && recieved == MODBUS_ADDRESS) {
    }
    _rx_len ++;*/
    
   
    // Clear interrupt flag
    IFS0bits.U1RXIF = 0;
}