/* 
 * File:   modbus.h
 * Author: 
 *
 * Created on 2 April 2015
 */

#ifndef MODBUS_H
#define MODBUS_H

void Modbus_RTU();
unsigned short Crc16(unsigned char * pcBlock, unsigned short len);


#endif /* MODBUS_H */
