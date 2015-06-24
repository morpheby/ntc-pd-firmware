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

#define __MB_TYPE(t) __MB_TYPE__##t
#define _MB_TYPE(t) __MB_TYPE(t)
#define _MB_SIZE(t) (sizeof(_MB_TYPE(t))/sizeof(uint16_t))
#define _MB_FUNCTION(f, t) f ## t

void modbus_regs_init();

void modbus_get_reg_data(uint16_t offset, uint16_t count, uint16_t *out,
        _Bool switchEndianess);
void modbus_set_reg_data(uint16_t offset, uint16_t count, const uint16_t *in,
        _Bool switchEndianess);

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
#define REG_ind_2_4             56, i16

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
#define REG_Status0            113, i16

#define MODBUS_DATA_END        114

#if APP_USE_MODBUS_EXT
#define REG_Bulk_Size          255, u16

#define MODBUS_BULK_START      256
#endif

#define _REG_OFFSET(r,t) r
#define _REG_TYPE(r,t) t

#define REG_TYPE(r) _REG_TYPE(r)
#define REG_OFFSET(r) _REG_OFFSET(r)

#define _REG_DEFINE(r) _MB_TYPE(REG_TYPE(REG_##r)) r

typedef struct _tagMODBUS_DATA {
    union {
        struct {     
            _REG_DEFINE(BRG_VAL);

            _REG_DEFINE(ADC0);
            _REG_DEFINE(ADC1);
            _REG_DEFINE(ADC2);
            _REG_DEFINE(ADC3);
            _REG_DEFINE(ADC4);
            _REG_DEFINE(ADC5);
            _REG_DEFINE(ADC6);

            _REG_DEFINE(K0);
            _REG_DEFINE(K1);
            _REG_DEFINE(K2);
            _REG_DEFINE(K3);
            _REG_DEFINE(K4);
            _REG_DEFINE(K5);
            _REG_DEFINE(K6);

            _REG_DEFINE(OFS_ADC0);
            _REG_DEFINE(OFS_ADC1);
            _REG_DEFINE(OFS_ADC2);
            _REG_DEFINE(OFS_ADC3);
            _REG_DEFINE(OFS_ADC4);
            _REG_DEFINE(OFS_ADC5);
            _REG_DEFINE(OFS_ADC6);

            _REG_DEFINE(n_a);
            _REG_DEFINE(n_b);
            _REG_DEFINE(n_c);
            _REG_DEFINE(ind_off);

            _REG_DEFINE(N);

            _REG_DEFINE(D_In);
            _REG_DEFINE(D_Out);

            _REG_DEFINE(Ind_Delay);

            _REG_DEFINE(profile);
            _REG_DEFINE(ind_0_1);
            _REG_DEFINE(ind_1_1);
            _REG_DEFINE(ind_2_1);
            _REG_DEFINE(ind_0_2);
            _REG_DEFINE(ind_1_2);
            _REG_DEFINE(ind_2_2);
            _REG_DEFINE(ind_0_3);
            _REG_DEFINE(ind_1_3);
            _REG_DEFINE(ind_2_3);
            _REG_DEFINE(ind_0_4);
            _REG_DEFINE(ind_1_4);
            _REG_DEFINE(ind_2_4);

            _REG_DEFINE(P1);
            _REG_DEFINE(P2);
            _REG_DEFINE(P3);

            _REG_DEFINE(Q1);
            _REG_DEFINE(Q2);
            _REG_DEFINE(Q3);

            _REG_DEFINE(S1);
            _REG_DEFINE(S2);
            _REG_DEFINE(S3);

            _REG_DEFINE(cos_f1);
            _REG_DEFINE(cos_f2);
            _REG_DEFINE(cos_f3);

            _REG_DEFINE(AD4_AVG);
            _REG_DEFINE(AD5_AVG);
            _REG_DEFINE(AD6_AVG);

            _REG_DEFINE(AD4_RMS);
            _REG_DEFINE(AD5_RMS);
            _REG_DEFINE(AD6_RMS);

            _REG_DEFINE(FLASH_WR);
            _REG_DEFINE(FLASH_RD);

            _REG_DEFINE(PROF_CHANGE_SOURCE);

            _REG_DEFINE(A0);
            _REG_DEFINE(A1);
            _REG_DEFINE(A2);
            _REG_DEFINE(A3);
            _REG_DEFINE(A4);
            _REG_DEFINE(A5);
            _REG_DEFINE(A6);

            _REG_DEFINE(ind_off_1);
            _REG_DEFINE(ind_off_2);
            _REG_DEFINE(ind_off_3);
            _REG_DEFINE(ind_off_4);

            _REG_DEFINE(channel_num_U);
            _REG_DEFINE(imp_kol);
            _REG_DEFINE(V);
            _REG_DEFINE(D_Out_Init);
            _REG_DEFINE(AD_12b);

            _REG_DEFINE(Control0);
            _REG_DEFINE(Status0);
            
            uint16_t __last1;
        };
        struct {
            uint16_t data[MODBUS_DATA_END];
            uint16_t __last2;
        };
    };
} _ModbusData_t;

#ifdef MODBUS_INTERNAL
#define MODBUS
#else
#define MODBUS extern
#endif

MODBUS _ModbusData_t MB;

#endif	/* MODBUS_REGISTERS_H */

