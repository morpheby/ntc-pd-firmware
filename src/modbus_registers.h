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

#define REG_DS1820_TEMP_1         112, f
#define REG_DS1820_TEMP_2         114, f
#define REG_DS1820_TEMP_3         116, f
#define REG_DS1820_TEMP_4         118, f
#define REG_DS1820_TEMP_5         120, f
#define REG_DS1820_TEMP_6         122, f
#define REG_DS1820_TEMP_7         124, f
#define REG_DS1820_TEMP_8         126, f
#define REG_DS1820_TEMP_9         128, f
#define REG_DS1820_TEMP_10         130, f
#define REG_DS1820_TEMP_11         132, f

#define REG_DI0_ImpFrequency         134, f
#define REG_DI1_ImpFrequency         136, f

#define REG_TermoId_0_bytes_0_1      138, u16
#define REG_TermoId_0_bytes_2_3      139, u16
#define REG_TermoId_0_bytes_4_5      140, u16
#define REG_TermoId_0_bytes_6_7      141, u16

#define REG_TermoId_1_bytes_0_1      142, u16
#define REG_TermoId_1_bytes_2_3      143, u16
#define REG_TermoId_1_bytes_4_5      144, u16
#define REG_TermoId_1_bytes_6_7      145, u16

#define REG_TermoId_2_bytes_0_1      146, u16
#define REG_TermoId_2_bytes_2_3      147, u16
#define REG_TermoId_2_bytes_4_5      148, u16
#define REG_TermoId_2_bytes_6_7      149, u16

#define REG_TermoId_3_bytes_0_1      150, u16
#define REG_TermoId_3_bytes_2_3      151, u16
#define REG_TermoId_3_bytes_4_5      152, u16
#define REG_TermoId_3_bytes_6_7      153, u16

#define REG_TermoId_4_bytes_0_1      154, u16
#define REG_TermoId_4_bytes_2_3      155, u16
#define REG_TermoId_4_bytes_4_5      156, u16
#define REG_TermoId_4_bytes_6_7      157, u16

#define REG_TermoId_5_bytes_0_1      158, u16
#define REG_TermoId_5_bytes_2_3      159, u16
#define REG_TermoId_5_bytes_4_5      160, u16
#define REG_TermoId_5_bytes_6_7      161, u16

#define REG_TermoId_6_bytes_0_1      162, u16
#define REG_TermoId_6_bytes_2_3      163, u16
#define REG_TermoId_6_bytes_4_5      164, u16
#define REG_TermoId_6_bytes_6_7      165, u16

#define REG_TermoId_7_bytes_0_1      166, u16
#define REG_TermoId_7_bytes_2_3      167, u16
#define REG_TermoId_7_bytes_4_5      168, u16
#define REG_TermoId_7_bytes_6_7      169, u16

#define REG_TermoId_8_bytes_0_1      170, u16
#define REG_TermoId_8_bytes_2_3      171, u16
#define REG_TermoId_8_bytes_4_5      172, u16
#define REG_TermoId_8_bytes_6_7      173, u16

#define REG_TermoId_9_bytes_0_1      174, u16
#define REG_TermoId_9_bytes_2_3      175, u16
#define REG_TermoId_9_bytes_4_5      176, u16
#define REG_TermoId_9_bytes_6_7      177, u16

#define REG_TermoId_10_bytes_0_1      178, u16
#define REG_TermoId_10_bytes_2_3      179, u16
#define REG_TermoId_10_bytes_4_5      180, u16
#define REG_TermoId_10_bytes_6_7      181, u16

#define REG_TermoCount             182, u16

#define REG_T_hot_min         183, f
#define REG_delta_T_hist         185, f

#define REG_Env_TermoId_bytes_0_1      187, u16
#define REG_Env_TermoId_bytes_2_3      188, u16
#define REG_Env_TermoId_bytes_4_5      189, u16
#define REG_Env_TermoId_bytes_6_7      190, u16

#define REG_Hot_TermoId_bytes_0_1      191, u16
#define REG_Hot_TermoId_bytes_2_3      192, u16
#define REG_Hot_TermoId_bytes_4_5      193, u16
#define REG_Hot_TermoId_bytes_6_7      194, u16

#define REG_Cold_TermoId_bytes_0_1      195, u16
#define REG_Cold_TermoId_bytes_2_3      196, u16
#define REG_Cold_TermoId_bytes_4_5      197, u16
#define REG_Cold_TermoId_bytes_6_7      198, u16

#define REG_DI2_ImpFrequency         199, f

#define REG_DI0_ImpCoef         201, f
#define REG_DI1_ImpCoef         203, f
#define REG_DI2_ImpCoef         205, f

#define MODBUS_DATA_END        207

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

            _REG_DEFINE(DS1820_TEMP_1);
            _REG_DEFINE(DS1820_TEMP_2);
            _REG_DEFINE(DS1820_TEMP_3);
            _REG_DEFINE(DS1820_TEMP_4);
            _REG_DEFINE(DS1820_TEMP_5);
            _REG_DEFINE(DS1820_TEMP_6);
            _REG_DEFINE(DS1820_TEMP_7);
            _REG_DEFINE(DS1820_TEMP_8);
            _REG_DEFINE(DS1820_TEMP_9);
            _REG_DEFINE(DS1820_TEMP_10);
            _REG_DEFINE(DS1820_TEMP_11);
            _REG_DEFINE(DI0_ImpFrequency);
            _REG_DEFINE(DI1_ImpFrequency);
            
            _REG_DEFINE(TermoId_0_bytes_0_1);
            _REG_DEFINE(TermoId_0_bytes_2_3);
            _REG_DEFINE(TermoId_0_bytes_4_5);
            _REG_DEFINE(TermoId_0_bytes_6_7);
            
            _REG_DEFINE(TermoId_1_bytes_0_1);
            _REG_DEFINE(TermoId_1_bytes_2_3);
            _REG_DEFINE(TermoId_1_bytes_4_5);
            _REG_DEFINE(TermoId_1_bytes_6_7);
            
            _REG_DEFINE(TermoId_2_bytes_0_1);
            _REG_DEFINE(TermoId_2_bytes_2_3);
            _REG_DEFINE(TermoId_2_bytes_4_5);
            _REG_DEFINE(TermoId_2_bytes_6_7);
            
            _REG_DEFINE(TermoId_3_bytes_0_1);
            _REG_DEFINE(TermoId_3_bytes_2_3);
            _REG_DEFINE(TermoId_3_bytes_4_5);
            _REG_DEFINE(TermoId_3_bytes_6_7);
            
            _REG_DEFINE(TermoId_4_bytes_0_1);
            _REG_DEFINE(TermoId_4_bytes_2_3);
            _REG_DEFINE(TermoId_4_bytes_4_5);
            _REG_DEFINE(TermoId_4_bytes_6_7);
            
            _REG_DEFINE(TermoId_5_bytes_0_1);
            _REG_DEFINE(TermoId_5_bytes_2_3);
            _REG_DEFINE(TermoId_5_bytes_4_5);
            _REG_DEFINE(TermoId_5_bytes_6_7);
            
            _REG_DEFINE(TermoId_6_bytes_0_1);
            _REG_DEFINE(TermoId_6_bytes_2_3);
            _REG_DEFINE(TermoId_6_bytes_4_5);
            _REG_DEFINE(TermoId_6_bytes_6_7);
            
            _REG_DEFINE(TermoId_7_bytes_0_1);
            _REG_DEFINE(TermoId_7_bytes_2_3);
            _REG_DEFINE(TermoId_7_bytes_4_5);
            _REG_DEFINE(TermoId_7_bytes_6_7);
            
            _REG_DEFINE(TermoId_8_bytes_0_1);
            _REG_DEFINE(TermoId_8_bytes_2_3);
            _REG_DEFINE(TermoId_8_bytes_4_5);
            _REG_DEFINE(TermoId_8_bytes_6_7);
            
            _REG_DEFINE(TermoId_9_bytes_0_1);
            _REG_DEFINE(TermoId_9_bytes_2_3);
            _REG_DEFINE(TermoId_9_bytes_4_5);
            _REG_DEFINE(TermoId_9_bytes_6_7);
            
            _REG_DEFINE(TermoId_10_bytes_0_1);
            _REG_DEFINE(TermoId_10_bytes_2_3);
            _REG_DEFINE(TermoId_10_bytes_4_5);
            _REG_DEFINE(TermoId_10_bytes_6_7);
                        
            _REG_DEFINE(TermoCount);
            
            _REG_DEFINE(T_hot_min);
            _REG_DEFINE(delta_T_hist);
            
            _REG_DEFINE(Env_TermoId_bytes_0_1);
            _REG_DEFINE(Env_TermoId_bytes_2_3);
            _REG_DEFINE(Env_TermoId_bytes_4_5);
            _REG_DEFINE(Env_TermoId_bytes_6_7);
            
            _REG_DEFINE(Hot_TermoId_bytes_0_1);
            _REG_DEFINE(Hot_TermoId_bytes_2_3);
            _REG_DEFINE(Hot_TermoId_bytes_4_5);
            _REG_DEFINE(Hot_TermoId_bytes_6_7);
            
            _REG_DEFINE(Cold_TermoId_bytes_0_1);
            _REG_DEFINE(Cold_TermoId_bytes_2_3);
            _REG_DEFINE(Cold_TermoId_bytes_4_5);
            _REG_DEFINE(Cold_TermoId_bytes_6_7);
            
            _REG_DEFINE(DI2_ImpFrequency);
            _REG_DEFINE(DI0_ImpCoef);
            _REG_DEFINE(DI1_ImpCoef);
            _REG_DEFINE(DI2_ImpCoef);
            
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

