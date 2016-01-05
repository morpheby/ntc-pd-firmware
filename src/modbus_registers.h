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
    
    unsigned int ADC_M0;//REG 3
    unsigned int ADC_M1;//REG 4
    unsigned int ADC_M2;//REG 5
    unsigned int ADC_A0;//REG 6
    unsigned int ADC_A1;//REG 7
    unsigned int ADC_A2;//REG 8
    unsigned int ADC_A3;//REG 9
        
    int M0_OFFSET;//REG 10
    int M1_OFFSET;//REG 11
    int M2_OFFSET;//REG 12
    int A0_OFFSET;//REG 13
    int A1_OFFSET;//REG 14
    int A2_OFFSET;//REG 15
    int A3_OFFSET;//REG 16
    
    float M0_Coef;//REG 17 REG 18
    float M1_Coef;//REG 19 REG 20
    float M2_Coef;//REG 21 REG 22
    float A0_Coef;//REG 23 REG 24
    float A1_Coef;//REG 25 REG 26
    float A2_Coef;//REG 27 REG 28
    float A3_Coef;//REG 29 REG 30
    
    float M0_value;//REG 31 REG 32
    float M1_value;//REG 33 REG 34
    float M2_value;//REG 35 REG 36
    float A0_value;//REG 37 REG 38
    float A1_value;//REG 39 REG 40
    float A2_value;//REG 41 REG 42
    float A3_value;//REG 43 REG 44
    
    float M0_AVG;//REG 45 REG 46
    float M1_AVG;//REG 47 REG 48
    float M2_AVG;//REG 49 REG 50
    float A0_AVG;//REG 51 REG 52
    float A1_AVG;//REG 53 REG 54
    float A2_AVG;//REG 55 REG 56
    float A3_AVG;//REG 57 REG 58
    
    float M0_RMS;//REG 59 REG 60
    float M1_RMS;//REG 61 REG 62
    float M2_RMS;//REG 63 REG 64
    float A0_RMS;//REG 65 REG 66
    float A1_RMS;//REG 67 REG 68
    float A2_RMS;//REG 69 REG 70
    float A3_RMS;//REG 71 REG 72
    
    unsigned int N;              //REG 73

    unsigned int D_In;          //REG 74
    unsigned int D_Out;          //REG 75
    unsigned int D_OutInit; //REG 76
    
    float DI0_ImpFreq; //REG 77 REG 78
    float DI1_ImpFreq; //REG 79 REG 80
    float DI2_ImpFreq; //REG 81 REG 82
    float DI3_ImpFreq; //REG 83 REG 84
    
    float DI0_ImpFreqCoef; //REG 85 REG 86
    float DI1_ImpFreqCoef; //REG 87 REG 88
    float DI2_ImpFreqCoef; //REG 80 REG 90
    float DI3_ImpFreqCoef; //REG 91 REG 92
    
    unsigned long int DI0_ImpCount; //REG 93 REG 94
    unsigned long int DI1_ImpCount; //REG 95 REG 96
    unsigned long int DI2_ImpCount; //REG 97 REG 98
    unsigned long int DI3_ImpCount; //REG 99 REG 100
    
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

#define MB_REGS_COUNT 93

extern struct modbus_t MB __attribute__ ((address(RAM_START_ADDRESS+0)));

void modbus_regs_init();

#endif	/* MODBUS_REGISTERS_H */

