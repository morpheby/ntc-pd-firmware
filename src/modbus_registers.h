/* 
 * File:   modbus_registers.h
 * Author: morpheby
 *
 * Created on аўторак, 23, чрвеня 2015, 16.42
 */

#ifndef MODBUS_REGISTERS_H
#define	MODBUS_REGISTERS_H

#include "system.h"
#include "app_connector.h"

#define __MB_TYPE__i16      int16_t
#define __MB_TYPE__u16      uint16_t
#define __MB_TYPE__f        float

#define _MB_TYPE(t) __MB_TYPE__##t
#define _MB_SIZE(t) (sizeof(_MB_TYPE(t))/sizeof(uint16_t))
#define _MB_FUNCTION(f, t) f ## t

void modbus_init();

void modbus_get_reg_data(uint16_t offset, uint16_t count, uint16_t *out);
void modbus_set_reg_data(uint16_t offset, uint16_t count, const uint16_t *in);

#define _MB_GET_REG_DECL(t) \
_MB_TYPE(t) modbus_get_reg_##t(uint16_t offset)

_MB_GET_REG_DECL(i16);
_MB_GET_REG_DECL(u16);
_MB_GET_REG_DECL(f);

#define _MB_SET_REG_DECL(t) \
void modbus_set_reg_##t(uint16_t offset, _MB_TYPE(t) value)

_MB_SET_REG_DECL(i16);
_MB_SET_REG_DECL(u16);
_MB_SET_REG_DECL(f);

#if APP_USE_MODBUS_EXT
void modbus_mmap_set(void *ptr, uint16_t size);
void modbus_mmap_free();
#endif

#define REG_BRG_VAL              0, i16
    
#define REG_ADC0                 1, f
#define REG_ADC1                 3, f
#define REG_ADC2                 5, f
#define REG_ADC3                 7, f
#define REG_ADC4                 9, f
#define REG_ADC5                11, f
#define REG_ADC6                13, f

#define REG_K0                  15, f
#define REG_K1                  17, f
#define REG_K2                  19, f
#define REG_K3                  21, f
#define REG_K4                  23, f
#define REG_K5                  25, f
#define REG_K6                  27, f

#define REG_OFS_ADC0            29, i16
#define REG_OFS_ADC1            30, i16
#define REG_OFS_ADC2            31, i16
#define REG_OFS_ADC3            32, i16
#define REG_OFS_ADC4            33, i16
#define REG_OFS_ADC5            34, i16
#define REG_OFS_ADC6            35, i16

#define REG_n_a                 36, i16
#define REG_n_b                 37, i16
#define REG_n_c                 38, i16
#define REG_ind_off             39, i16

#define REG_N                   40, i16

#define REG_D_In                41, i16
#define REG_D_Out               42, i16

#define REG_Ind_Delay           43, i16

#define REG_profile             44, i16
#define REG_ind_0_1             45, i16
#define REG_ind_1_1             46, i16
#define REG_ind_2_1             47, i16
#define REG_ind_0_2             48, i16
#define REG_ind_1_2             49, i16
#define REG_ind_2_2             50, i16
#define REG_ind_0_3             51, i16
#define REG_ind_1_3             52, i16
#define REG_ind_2_3             53, i16
#define REG_ind_0_4             54, i16
#define REG_ind_1_4             55, i16
#define REG_ind_1_4             56, i16

#define REG_P1                  57, f
#define REG_P2                  59, f
#define REG_P3                  61, f

#define REG_Q1                  63, f
#define REG_Q2                  65, f
#define REG_Q3                  67, f

#define REG_S1                  69, f
#define REG_S2                  71, f
#define REG_S3                  73, f

#define REG_cos_f1              75, f
#define REG_cos_f2              77, f
#define REG_cos_f3              79, f

#define REG_AD4_AVG             81, f
#define REG_AD5_AVG             83, f
#define REG_AD6_AVG             85, f

#define REG_AD4_RMS             87, f
#define REG_AD5_RMS             89, f
#define REG_AD6_RMS             91, f

#define REG_FLASH_WR            93, i16
#define REG_FLASH_RD            94, i16

#define REG_PROF_CHANGE_SOURCE  95, i16

#define REG_A0                  96, u16
#define REG_A1                  97, u16
#define REG_A2                  98, u16
#define REG_A3                  99, u16
#define REG_A4                 100, u16
#define REG_A5                 101, u16
#define REG_A6                 102, u16

#define REG_ind_off_1          103, i16
#define REG_ind_off_2          104, i16
#define REG_ind_off_3          105, i16
#define REG_ind_off_4          106, i16

#define REG_channel_num_U      107, i16
#define REG_imp_kol            108, i16
#define REG_V                  109, i16
#define REG_D_Out_Init         110, i16
#define REG_AD_12b             111, i16

#define REG_Control0           112, i16

#define MODBUS_DATA_END        113

#if APP_USE_MODBUS_EXT
#define REG_Bulk_Size          255, u16

#define MODBUS_BULK_START      256
#endif

#define REG_OFFSET(r,t) r
#define REG_TYPE(r,t) t

#endif	/* MODBUS_REGISTERS_H */

