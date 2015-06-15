
#include "cn_inputs.h"
#include "ipl-config.h"
#include "board-config.h"

#define CNI_THRESHOLD            0

#define __CNI_TRIS(n) TRIS_BIT(CNI_##n##_PIN_TYPE, CNI_##n##_PIN_NUM)
#define __CNI_IE_(n16,n) CNEN##n16##bits.CN##n##IE
#define __CNI_IE(n16,n) __CNI_IE_(n16,n)
#define __CNI_PULLUP_(n16,n) CNPU##n16##bits.CN##n##PUE
#define __CNI_PULLUP(n16,n) __CNI_PULLUP_(n16,n)
#define __CNI_NEEDSPULLUP(n) CNI_##n##_NEEDSPU
#define __CNI_PORT(n) PIN_PORT(CNI_##n##_PIN_TYPE, \
                                    CNI_##n##_PIN_NUM)

#define _CNI_TRIS(n) __CNI_TRIS(n)
#define _CNI_IE(n) __CNI_IE(CNI_##n##_16, CNI_##n)
#define _CNI_PULLUP(n) __CNI_PULLUP(CNI_##n##_16, CNI_##n)
#define _CNI_NEEDSPULLUP(n) __CNI_NEEDSPULLUP(n)
#define _CNI_PORT(n) __CNI_PORT(n)

#define __CNI_INIT(n)                             \
    do {                                        \
        _CNI_TRIS(n) = 1;                       \
        _CNI_IE(n) = 1;                         \
        _CNI_PULLUP(n) = _CNI_NEEDSPULLUP(n);   \
    }while(0)

#define _CNI_INIT(n) __CNI_INIT(n)

/* Public file, containing all declarations and necessary fns and types */
#include "cn_inputs_reg.h"

CNI_DECL_PROC_FN(1);

#define _CNI_SIGNAL(n) \
    CNI_CALL_PROC_FN(n, _CNI_PORT(n))

void cni_init() {
    _CNI_INIT(1);

    IFS1bits.CNIF = 0;
    IPC4bits.CNIP = IPL_CN_INPUT;
    IEC1bits.CNIE = 1;
}

void _ISR_NOPSV _CNInterrupt() {
    int i;

    for(i = 0; i < CNI_THRESHOLD; ++i);
    _CNI_SIGNAL(1);
    IFS1bits.CNIF = 0;
}
