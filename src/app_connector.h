/* 
 * File:   app_connector.h
 * Author: morpheby
 *
 * Created on пятніца, 19, чрвеня 2015, 11.03
 */

#ifndef APP_CONNECTOR_H
#define	APP_CONNECTOR_H

#include "app_main.h"

#define IF_0(a)
#define IF_1(a) a
#define __IF(x, a) IF_##x(a)
#define IF(x, a) __IF(x, a)

#if APP_USE_ADC
#define ADC_DECL_PROC_FN(channel, varValue) \
    void __adc_##channel##_proc(uint16_t value)

#define ADC_CALL_PROC_FN(channel, value) \
    __adc_##channel##_proc(value)
#else
#define ADC_CALL_PROC_FN(channel, value)
#endif

#define __CNI_DECL_PROC_FN(n, varOn) \
    void __cni_##n##_proc(_Bool varOn)
#define CNI_DECL_PROC_FN_INTERNAL(n) \
    IF(CNI_##n##_ENABLE, void __cni_##n##_proc(_Bool));
#define CNI_DECL_PROC_FN(n, varOn) __CNI_DECL_PROC_FN(n, varOn)

#define __CNI_CALL_PROC_FN(n, on)  \
    IF(CNI_##n##_ENABLE, __cni_##n##_proc(on))
#define CNI_CALL_PROC_FN(n, on) __CNI_CALL_PROC_FN(n, on)



#endif	/* APP_CONNECTOR_H */

