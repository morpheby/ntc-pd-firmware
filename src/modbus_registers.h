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

#define REG_N                   36, u16

#define REG_D_In                37, i16
#define REG_D_Out               38, i16

#define REG_Ind_Delay           39, i16

#define REG_profile             40, i16
#define REG_ind_0_1             41, i16
#define REG_ind_1_1             42, i16
#define REG_ind_2_1             43, i16
#define REG_ind_0_2             44, i16
#define REG_ind_1_2             45, i16
#define REG_ind_2_2             46, i16
#define REG_ind_0_3             47, i16
#define REG_ind_1_3             48, i16
#define REG_ind_2_3             49, i16
#define REG_ind_0_4             50, i16
#define REG_ind_1_4             51, i16
#define REG_ind_2_4             52, i16

#define REG_P1                  53, f
#define REG_P2                  55, f
#define REG_P3                  57, f

#define REG_Q1                  59, f
#define REG_Q2                  61, f
#define REG_Q3                  63, f

#define REG_S1                  65, f
#define REG_S2                  67, f
#define REG_S3                  69, f

#define REG_cos_f1              71, f
#define REG_cos_f2              73, f
#define REG_cos_f3              75, f

#define REG_M0_AVG              77, f
#define REG_M1_AVG              79, f
#define REG_M2_AVG              81, f

#define REG_M0_RMS              83, f
#define REG_M1_RMS              85, f
#define REG_M2_RMS              87, f

#define REG_FLASH_WR            89, i16
#define REG_FLASH_RD            90, i16

#define REG_PROF_CHANGE_SOURCE  91, i16

#define REG_A0                  92, i16
#define REG_A1                  93, i16
#define REG_A2                  94, i16
#define REG_A3                  95, i16
#define REG_A4                 96, i16
#define REG_A5                 97, i16
#define REG_A6                 98, i16

#define REG_imp_kol            99, i16
#define REG_V                  100, f
#define REG_D_Out_Init         102, u16
#define REG_AD_12b             103, i16

#define REG_Control0           104, i16
#define REG_Status0            105, i16

#define REG_Power0             106, i16
#define REG_Position0          107, i16
#define REG_Position1          108, i16
#define REG_PositionK0         109, f

#define REG_QEI_POS         111, u16

#define REG_P1_coef          112, f
#define REG_P2_coef          114, f
#define REG_P3_coef          116, f

#define MODBUS_DATA_END        118

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

            _REG_DEFINE(M0_AVG);
            _REG_DEFINE(M1_AVG);
            _REG_DEFINE(M2_AVG);

            _REG_DEFINE(M0_RMS);
            _REG_DEFINE(M1_RMS);
            _REG_DEFINE(M2_RMS);

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

            _REG_DEFINE(imp_kol);
            _REG_DEFINE(V);
            _REG_DEFINE(D_Out_Init);
            _REG_DEFINE(AD_12b);

            _REG_DEFINE(Control0);
            _REG_DEFINE(Status0);
            
            _REG_DEFINE(Power0);
            _REG_DEFINE(Position0);
            _REG_DEFINE(Position1);
            _REG_DEFINE(PositionK0);

            _REG_DEFINE(QEI_POS);
            
            _REG_DEFINE(P1_coef);
            _REG_DEFINE(P2_coef);
            _REG_DEFINE(P3_coef);
            
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

