#include "cn_inputs.h"
#include "ipl-config.h"

// 1000 seems to be reasonable to filter
// out any interference and still to have
// no display flashes
#define CNI_THRESHOLD         1000

#define CNI_1_PIN_NUM            5
#define CNI_1_PIN_TYPE           B
#define CNI_1                   27
#define CNI_1_16                 2
#define CNI_1_NEEDSPU            0

#define CNI_2_PIN_NUM            6
#define CNI_2_PIN_TYPE           B
#define CNI_2                   24
#define CNI_2_16                 2
#define CNI_2_NEEDSPU            0

#define CNI_3_PIN_NUM            7
#define CNI_3_PIN_TYPE           B
#define CNI_3                   23
#define CNI_3_16                 2
#define CNI_3_NEEDSPU            0

#define CNI_4_PIN_NUM            8
#define CNI_4_PIN_TYPE           B
#define CNI_4                   22
#define CNI_4_16                 2
#define CNI_4_NEEDSPU            0

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

/* Public file, containing all declarations ao neccessary fns and types */
#include "cn_inputs_reg.h"

CNI_DECL_PROC_FN(1);
CNI_DECL_PROC_FN(2);
CNI_DECL_PROC_FN(3);
CNI_DECL_PROC_FN(4);

#define _CNI_SIGNAL(n) \
    CNI_CALL_PROC_FN(n, _CNI_PORT(n))

void cni_init() {
    _CNI_INIT(1);
    _CNI_INIT(2);
    _CNI_INIT(3);
    _CNI_INIT(4);

    IFS1bits.CNIF = 0;
    IPC4bits.CNIP = IPL_CN_INPUT;
    IEC1bits.CNIE = 1;
}

void _ISR_NOPSV _CNInterrupt() {
    int i;

    for(i = 0; i < CNI_THRESHOLD; ++i);
    _CNI_SIGNAL(1);
    _CNI_SIGNAL(2);
    _CNI_SIGNAL(3);
    _CNI_SIGNAL(4);
    IFS1bits.CNIF = 0;
}
