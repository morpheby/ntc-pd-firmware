/* 
 * File:   DS1820.h
 * Author: Евгений Кравченко
 *
 * Created on 27 Август 2015 г., 8:06
 */

#ifndef DS1820_H
#define	DS1820_H

#include "system.h"

#define DS1820_SENSOR_COUNT 11

#define IS_9_BIT_SENSOR 0 //ds1820 is 9 bit sensor (0.5 °C precision). ds18b20 is 12 bit sensor (0.0625 °C prscision))

void DS1820_initROM();
uint16_t DS1820_getIdWord(uint8_t deviceIndex, uint8_t wordIndex);

void DS1820_setLineToZero();
void DS1820_setLineToOne();
bool DS1820_lineState();

void DS1820_update();
float DS1820_temperature(unsigned int index);
void DS1820_init();
void DS1820_TX(unsigned char cmd);
unsigned char DS1820_RX();

#endif	/* DS1820_H */

