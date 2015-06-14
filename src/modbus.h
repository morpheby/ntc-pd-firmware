/* 
 * File:   modbus.h
 * Author: 
 *
 * Created on 2 April 2015
 */

#ifndef MODBUS_H
#define MODBUS_H

void RS_Send(int* buf,unsigned char size,unsigned char ADR, char Op_Code);

char RS_CheckCRC();
void RS_Reset();
void RS_Update();

unsigned short Crc16(unsigned char * pcBlock, unsigned short len);


#endif /* MODBUS_H */
