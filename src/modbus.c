
#include "modbus.h"
#include "system.h"
#include "uart_base.h"
#include "app_connector.h"
#include "timing.h"
#include "modbus_registers.h"
#include "util.h"
#include <string.h>

#define TIMEOUT		200 // msec
#define TX_BUFFER_SIZE	64
#define RX_BUFFER_SIZE  64

// Modbus commands
#define READ_HOLDING_REGISTERS		0x03
#define PRESET_SINGLE_REGISTER		0x06
#define PRESET_MULTIPLE_REGISTERS	0x10

static uint8_t _rx_buf[RX_BUFFER_SIZE];
static uint8_t _rx_len = 0;
static uint8_t _tx_buf[TX_BUFFER_SIZE];
static uint8_t _tx_len = 0;
static uint8_t _tx_pos = 0;

#define MB_STATE_READY 0
#define MB_STATE_CMD_WAIT 1
#define MB_STATE_DATA_WAIT 2

static uint8_t _mb_state = MB_STATE_READY;
static uint8_t address = DEFAULT_MODBUS_ADDRESS;

unsigned char CRC_16_Hi,CRC_16_Lo;

// Fast CRC16 table
const unsigned short Crc16Table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

inline uint16_t crc16_kern(uint16_t crc0, uint8_t byte) {
    return (crc0 >> 8) ^ Crc16Table[(crc0 & 0xFF) ^ byte];
}

uint16_t crc16_block_add(uint16_t crc, uint8_t *data, uint16_t size) {
    while (size--) {
        crc = crc16_kern(crc, *data++);
    }
    return crc;
}

unsigned short Crc16(unsigned char *pcBlock, unsigned short len) {
    return crc16_block_add(0xFFFF, pcBlock, len);
}
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

void Modbus_RTU() {
    stop_mb_silence_timer();
    if(crcCorrect(_rx_buf, _rx_len)) {
        uint16_t* mbDataPtr = &MB.ADDRESS;
        unsigned int start = (_rx_buf[2] << 8) | _rx_buf[3];
        unsigned int count = (_rx_buf[4] << 8) | _rx_buf[5];
        uint8_t regIndex;
        switch(_rx_buf[1]) {
            case READ_HOLDING_REGISTERS:
            {
                IEC0bits.U1TXIE=0;                       
                _tx_buf[0] = address;
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
                _tx_buf[0] = address;
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
                _tx_buf[0] = address;
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
            if(recieved == address) {
                _rx_buf[_rx_len] = recieved;
                _rx_len ++;
                _mb_state = MB_STATE_CMD_WAIT;    
                start_mb_silence_timer();
            }
            break;
        }
        case (MB_STATE_CMD_WAIT):
        {
            switch (recieved) {
                case READ_HOLDING_REGISTERS:
                {
                    _mb_state = MB_STATE_DATA_WAIT;      
                    start_mb_silence_timer();
                    break;
                }
                case PRESET_SINGLE_REGISTER:
                {
                    _mb_state = MB_STATE_DATA_WAIT; 
                    start_mb_silence_timer();
                    break;
                }
                case PRESET_MULTIPLE_REGISTERS:
                {
                    _mb_state = MB_STATE_DATA_WAIT; 
                    start_mb_silence_timer();
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
            start_mb_silence_timer();
            _rx_buf[_rx_len] = recieved;
            _rx_len++;
            break;
        }
    }
       
    // Clear interrupt flag
    IFS0bits.U1RXIF = 0;
}

void setModbusAddress(unsigned char addr)
{
    address = addr;
}