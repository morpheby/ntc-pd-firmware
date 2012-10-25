/* 
 * File:   names_ext.h
 * Author: Morphe
 *
 * Created on 6 ???????? 2012 ?., 17:10
 */

#ifndef NAMES_EXT_H
#define	NAMES_EXT_H

#include "system.h"

#define NM_NAME_T_DOUBLE        0x0000
#define NM_NAME_T_INT64         0x0001
#define NM_NAME_T_WORD          0x0003
#define NM_NAME_T_BOOL          0x0007

#define NM_NAME_T_VALUE         0x0000
#define NM_NAME_T_ARRAY         0x0010
#define NM_NAME_T_SETTABLE      0x0020
#define NM_NAME_T_READABLE      0x0040

#define NM_NAME_T_RPC           0xFFFF


#define NM_AVAILABLE_NAMES      5

#define NM_INPUT_0              0
#define NM_INPUT_0_TYPE         NM_NAME_T_DOUBLE | NM_NAME_T_VALUE | NM_NAME_T_READABLE
#define NM_INPUT_1              1

#define _NM_CONST_CFG_INIT                              \
    const uint16_t _nmConstCfg[NM_AVAILABLE_NAMES] = {  \
                        NM_INPUT_0_TYPE                 \
    }


#endif	/* NAMES_EXT_H */

