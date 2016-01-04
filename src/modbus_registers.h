/* 
 * File:   modbus_registers.h
 * Author: morpheby
 *
 * Created on аўторак, 23, чрвеня 2015, 16.42
 */

#ifndef MODBUS_REGISTERS_H
#define	MODBUS_REGISTERS_H

#include "system.h"

float modbus_get_reg_f(uint8_t regNumber);
uint16_t modbus_get_reg_u16(uint8_t regNumber);
int16_t modbus_get_reg_i16(uint8_t regNumber);

#define RAM_START_ADDRESS               0x900

struct modbus_t {
    unsigned int ADDRESS;//REG 0
    unsigned int FLASH_WRITE;//REG 1
    unsigned int RESET;//REG 2
    int N;              //REG 40

    int D_In;          //REG 41
    int D_Out;          //REG 42
    int Ind_Delay;      //REG 43

    int profile;        //REG 44
    int ind_0_1;        //REG 45
    int ind_1_1;        //REG 46
    int ind_2_1;        //REG 47
    int ind_0_2;        //REG 48
    int ind_1_2;        //REG 49
    int ind_2_2;       //REG 50
    int ind_0_3;       //REG 51
    int ind_1_3;       //REG 52
    int ind_2_3;       //REG 53
    int ind_0_4;       //REG 54
    int ind_1_4;       //REG 55
    int ind_2_4;      //REG 56

    int imp_kol;           //REG 108
    int PROF_CHANGE_SOURCE;//REG 95
};

#define MB_REGS_COUNT 20

extern struct modbus_t MB __attribute__ ((address(RAM_START_ADDRESS+0)));

void modbus_regs_init();

#endif	/* MODBUS_REGISTERS_H */

