#include "DS1820.h"
#include "board-config.h"
#include "modbus_registers.h"

#define STATE_DEFAULT 0
#define STATE_DATA_READ 1

static uint8_t state = STATE_DEFAULT;
static unsigned char LS_byte;
static unsigned char MS_byte;

static float *temperature;
static bool init_ok;
static unsigned int currentIndex = 0;

static bool hasROMConflicts;
static uint8_t lastROMConflictIndex;
static uint8_t currentROMConflictIndex;
static uint8_t *ROM;

bool DS1820_lineState() {
    return (PIN_PORT(DI3_PIN_TYPE, DI3_PIN_NUM) == 1);
}

void DS1820_setLineToZero()
{    
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = 1;
}
void DS1820_setLineToOne()
{
    PIN_LATCH(VT0_PIN_TYPE, VT0_PIN_NUM) = 0;    
}

void DS1820_update() 
{
    switch(state)
    {
        case STATE_DEFAULT:
        {
            DS1820_init();
            if(init_ok) 
            {
                DS1820_TX(0xCC);
                DS1820_TX(0x44);
                state = STATE_DATA_READ;
            }
            break;
        }
        case STATE_DATA_READ:
        {            
            if(DS1820_lineState()) {
                DS1820_init();
                if(init_ok) {
                    uint8_t i;
                    DS1820_TX(0x55);
                    unsigned int offset = currentIndex*8;
                    for(i = 0; i < 8; ++i) {
                        DS1820_TX(ROM[i + offset]);
                    }                    
                    currentIndex++;
                    if(currentIndex >= MB.TermoCount) {
                        currentIndex = 0;
                    }                        

                    DS1820_TX(0xBE);
                    LS_byte = DS1820_RX();
                    MS_byte = DS1820_RX();
                    MS_byte = (MS_byte << 3);
                    MS_byte += (LS_byte >> 5);
                    LS_byte = (LS_byte << 3);
                    int8_t temp = (MS_byte << 4) + (LS_byte >> 4);
                    temperature[currentIndex] = (float)(temp) + 0.0625f * (LS_byte & 0b00001111);
                }
                state = STATE_DEFAULT;
            }
            break;
        }
    }
}

float DS1820_temperature(unsigned int index)
{
    return temperature[index];
}

void DS1820_init()
{
    init_ok = false;
    SYSHANDLE handle = high_priority_enter();
    DS1820_setLineToZero();
    delay_us(500);
    DS1820_setLineToOne();
    delay_us(50);
    if(!DS1820_lineState()) {
        init_ok = true;
    }
    delay_us(500);
    high_priority_exit(handle);    
}

void DS1820_TX(unsigned char cmd)
{
    unsigned char tmp = cmd;
    uint8_t i = 0;
    for(i = 0; i < 8; ++i) {
        if (tmp&0x01) {
            DS1820_writeOne();
        } else {
            DS1820_writeZero();
        }
        tmp >>= 1;
    } 
}

unsigned char DS1820_RX()
{
    SYSHANDLE handle;    
    unsigned char d = 0;
    unsigned char i = 0;
    for (i=0;i<8;i++){
        handle = high_priority_enter();
        DS1820_setLineToZero();//прижимаем линию
        delay_us(1);
        DS1820_setLineToOne();
        delay_us(5);
        d>>=1;                     //освобождаем место под новый бит
        if (DS1820_lineState())
        {  
            d |= 0x80;   //если 1 то записываем 1
        }
        high_priority_exit(handle);
        delay_us(60);            //ждем до положенного времени
    }
    return d;
}

void DS1820_findNextROM()
{
    SYSHANDLE handle;
    unsigned int i;
    unsigned int offset = currentIndex*8;
    
    hasROMConflicts = 0;
    currentROMConflictIndex = 0;
    
    DS1820_init();
    if(init_ok) {
        MB.TermoCount++;
        bool bit_1 = 0;
        bool bit_2 = 0;
        DS1820_TX(0xF0);
                
        for(i = 0; i < 64; ++i) {
            handle = high_priority_enter();
            DS1820_setLineToZero();//прижимаем линию
            delay_us(1);
            DS1820_setLineToOne();
            delay_us(5);
            bit_1 = DS1820_lineState();
            high_priority_exit(handle);
            delay_us(60);            //ждем до положенного времени
        
            handle = high_priority_enter();
            DS1820_setLineToZero();//прижимаем линию
            delay_us(1);
            DS1820_setLineToOne();
            delay_us(5);
            bit_2 = DS1820_lineState();
            high_priority_exit(handle);
            delay_us(60);            //ждем до положенного времени
           
            bool ROM_bit = false;
            if(bit_1 && !bit_2) {
                //на всех активных устройствах в разряде 1
                ROM_bit = true;
            } else if (!bit_1 && bit_2) {
                //на всех активных устройствах в разряде 0
                ROM_bit = 0;                
            } else if(!bit_1 && !bit_2) {
                //конфликт 
                if(i == lastROMConflictIndex) {
                    ROM_bit = 1;
                } else {
                    hasROMConflicts = true;
                    if(i > lastROMConflictIndex) {
                        currentROMConflictIndex = i;
                        ROM_bit = 0;
                    } else {
                        if(ROM[offset] & 0x01) {
                            ROM_bit = 1;
                        } else {
                            ROM_bit = 0;
                            currentROMConflictIndex = i;
                        }
                    }                    
                }
            }
            
            if(ROM_bit) {
                DS1820_writeOne();
            } else {
                DS1820_writeZero();
            }
            
            ROM[0 + offset]>>=1;
			if (ROM[1 + offset] & 0b00000001)ROM[0 + offset] |= 0b10000000;
			ROM[1 + offset]>>=1;
			if (ROM[2 + offset] & 0b00000001)ROM[1 + offset] |= 0b10000000;		
			ROM[2 + offset]>>=1;
			if (ROM[3 + offset] & 0b00000001)ROM[2 + offset] |= 0b10000000;
			ROM[3 + offset]>>=1;
			if (ROM[4 + offset] & 0b00000001)ROM[3 + offset] |= 0b10000000;
			ROM[4 + offset]>>=1;
			if (ROM[5 + offset] & 0b00000001)ROM[4 + offset] |= 0b10000000;
			ROM[5 + offset]>>=1;
			if (ROM[6 + offset] & 0b00000001)ROM[5 + offset] |= 0b10000000;		
			ROM[6 + offset]>>=1;
			if (ROM[7 + offset] & 0b00000001)ROM[6 + offset] |= 0b10000000;
            ROM[7 + offset]>>=1;
            if(ROM_bit) {
                ROM[7 + offset] |= 0x80;                
            }
        }
	 	// определить текущее несоответствие последним
		lastROMConflictIndex=currentROMConflictIndex;
    }  
}

void DS1820_initROM()
{
    unsigned int i;
    ROM = &MB.TermoId_0_bytes_0_1;
    temperature = &MB.DS1820_TEMP_1;
    for(i = 0; i < 8 * DS1820_SENSOR_COUNT; ++i) {
        ROM[i] = 0;
        temperature[i] = 0;
    }
    MB.TermoCount = 0;    
    currentIndex = 0;
    lastROMConflictIndex = 65;
    
    do {
        DS1820_findNextROM();        
        currentIndex ++;
                
        if(currentIndex >= DS1820_SENSOR_COUNT) {
            break;
        }
    } while (hasROMConflicts);
    
    currentIndex = 0;
}

void DS1820_writeOne() 
{
    SYSHANDLE handle = high_priority_enter();
    
    DS1820_setLineToZero(); 
    delay_us(1);
    DS1820_setLineToOne();
    delay_us(60);
    high_priority_exit(handle);
}

void DS1820_writeZero()
{
    SYSHANDLE handle = high_priority_enter();
    
    DS1820_setLineToZero();
    delay_us(60);
    DS1820_setLineToOne();
    delay_us(1);
    high_priority_exit(handle);
}

uint16_t DS1820_getIdWord(uint8_t deviceIndex, uint8_t wordIndex)
{
    uint16_t result = 0;
    if(deviceIndex < MB.TermoCount && wordIndex < 4) {
        uint8_t offset = deviceIndex * 8 + wordIndex*2;
        result = (((uint16_t)ROM[offset]) << 8) + ROM[offset + 1];
    }
    
    return result;
}