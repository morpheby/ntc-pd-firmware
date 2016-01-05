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
    
    int ADC_M0;//REG 3
    int ADC_M1;//REG 4
    int ADC_M2;//REG 5
    int ADC_A0;//REG 6
    int ADC_A1;//REG 7
    int ADC_A2;//REG 8
    int ADC_A3;//REG 9
        
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
    
    unsigned int disk_imp_count;// REG 101
    float QEI_W_rad_s;// REG 102 REG 103
    float QEI_W_rot_s;// REG 104 REG 105
    
    unsigned int Ind_Delay;      //REG 106
    unsigned int PROF_CHANGE_SOURCE;//REG 107
    unsigned int profile;        //REG 108
    
    unsigned int ind_0_1;        //REG 109
    unsigned int ind_1_1;        //REG 110
    unsigned int ind_2_1;        //REG 111
    unsigned int ind_0_2;        //REG 112
    unsigned int ind_1_2;        //REG 113
    unsigned int ind_2_2;       //REG 114
    unsigned int ind_0_3;       //REG 115
    unsigned int ind_1_3;       //REG 116
    unsigned int ind_2_3;       //REG 117
    unsigned int ind_0_4;       //REG 118
    unsigned int ind_1_4;       //REG 119
    unsigned int ind_2_4;      //REG 120
    float TimerValue; //REG 121 REG 122
    
    float M0_RMS_sign_threshold; //REG 123 REG 124
    float M1_RMS_sign_threshold; //REG 125 REG 126
    float M2_RMS_sign_threshold; //REG 127 REG 128
    float A0_RMS_sign_threshold; //REG 129 REG 130
    float A1_RMS_sign_threshold; //REG 131 REG 132
    float A2_RMS_sign_threshold; //REG 133 REG 134
    float A3_RMS_sign_threshold; //REG 135 REG 136
    
    float P1;// REG 137 REG 138
    float P2;// REG 139 REG 140
    float P3;// REG 141 REG 142
    
    float Q1;// REG 137 REG 138
    float Q2;// REG 139 REG 140
    float Q3;// REG 141 REG 142
    
    float S1;// REG 143 REG 144
    float S2;// REG 145 REG 146
    float S3;// REG 147 REG 148
    
    float cos_f1;// REG 149 REG 150
    float cos_f2;// REG 151 REG 152
    float cos_f3;// REG 153 REG 154
    
    float P1_coef;// REG 155 REG 156
    float P2_coef;// REG 157 REG 158
    float P3_coef;// REG 159 REG 161    
};

#define MB_REGS_COUNT 161

extern struct modbus_t MB __attribute__ ((address(RAM_START_ADDRESS+0)));

void modbus_regs_init();

#endif	/* MODBUS_REGISTERS_H */

