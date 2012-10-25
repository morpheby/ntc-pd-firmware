/* 
 * File:   cn_inputs_reg.h
 * Author: Morphe
 *
 * Created on 27 ?????? 2012 ?., 16:22
 */

#ifndef CN_INPUTS_REG_H
#define	CN_INPUTS_REG_H

#define __CNI_DECL_PROC_FN(n) \
    void __cni_##n##_proc(_Bool __on)
#define CNI_DECL_PROC_FN(n) __CNI_DECL_PROC_FN(n)

#define __CNI_CALL_PROC_FN(n, on)  \
    __cni_##n##_proc(on)
#define CNI_CALL_PROC_FN(n, on) __CNI_CALL_PROC_FN(n, on)

#define CNI_BTN_LEFT      3
#define CNI_BTN_RIGHT    1
#define CNI_BTN_ENTER   2
#define CNI_BTN_RETURN  4

#endif	/* CN_INPUTS_REG_H */

