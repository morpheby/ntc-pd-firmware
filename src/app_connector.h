/* 
 * File:   app_connector.h
 * Author: morpheby
 *
 * Created on пятніца, 19, чрвеня 2015, 11.03
 */

#ifndef APP_CONNECTOR_H
#define	APP_CONNECTOR_H

/*
 * app_connector.h is header included in every module that has app-faced ends
 */

#include "app_main.h"

#define IF_0(a)
#define IF_1(a) a
#define __IF(x, a) IF_##x(a)
#define IF(x, a) __IF(x, a)

#define ADC_DECL_PROC_FN(channel, varValue) \
    void __adc_##channel##_proc(uint16_t value)

#define ADC_CALL_PROC_FN(channel, value) \
    __adc_##channel##_proc(value)

/*
 * CNI_DECL_PROC_FN(n, varOn) declares a function body for a signal in case of
 * CN event on nth pin, with the value of the event passed through the varOn
 * variable name. Each pin signal is enabled separately through the CNI_x_ENABLE
 * macro definition in board-config.h
 */
#define __CNI_DECL_PROC_FN(n, varOn) \
    IF(CNI_##n##_ENABLE, void __cni_##n##_proc(_Bool varOn))
#define CNI_DECL_PROC_FN(n, varOn) __CNI_DECL_PROC_FN(n, varOn)

#define __CNI_CALL_PROC_FN(n, on)  \
    IF(CNI_##n##_ENABLE, __cni_##n##_proc(on))
#define CNI_CALL_PROC_FN(n, on) __CNI_CALL_PROC_FN(n, on)

#define MAIN_DECL_LOOP_FN() \
    IF(MAIN_LOOP_FN_ENABLE, void __main_loop_fn())
#define MAIN_CALL_LOOP_FN() \
    IF(MAIN_LOOP_FN_ENABLE, __main_loop_fn())

#endif	/* APP_CONNECTOR_H */

