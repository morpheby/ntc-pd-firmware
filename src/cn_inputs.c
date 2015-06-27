
#include "cn_inputs.h"
#include "ipl-config.h"
#include "board-config.h"
#include "app_connector.h"

#define CNI_THRESHOLD            50

#define CNI_0_PIN_NUM            4
#define CNI_0_PIN_TYPE           A
#define CNI_0_16                 1

#define CNI_1_PIN_NUM            4
#define CNI_1_PIN_TYPE           B
#define CNI_1_16                 1

#define CNI_2_PIN_NUM            0
#define CNI_2_PIN_TYPE           A
#define CNI_2_16                 1

#define CNI_3_PIN_NUM            1
#define CNI_3_PIN_TYPE           A
#define CNI_3_16                 1

#define CNI_4_PIN_NUM            0
#define CNI_4_PIN_TYPE           B
#define CNI_4_16                 1

#define CNI_5_PIN_NUM            1
#define CNI_5_PIN_TYPE           B
#define CNI_5_16                 1

#define CNI_6_PIN_NUM            2
#define CNI_6_PIN_TYPE           B
#define CNI_6_16                 1

#define CNI_7_PIN_NUM            3
#define CNI_7_PIN_TYPE           B
#define CNI_7_16                 1

#define CNI_8_PIN_NUM            0
#define CNI_8_PIN_TYPE           C
#define CNI_8_16                 1

#define CNI_9_PIN_NUM            1
#define CNI_9_PIN_TYPE           C
#define CNI_9_16                 1

#define CNI_10_PIN_NUM           2
#define CNI_10_PIN_TYPE          C
#define CNI_10_16                1

#define CNI_11_PIN_NUM           15
#define CNI_11_PIN_TYPE          B
#define CNI_11_16                1

#define CNI_12_PIN_NUM           14
#define CNI_12_PIN_TYPE          B
#define CNI_12_16                1

#define CNI_13_PIN_NUM           13
#define CNI_13_PIN_TYPE          B
#define CNI_13_16                1

#define CNI_14_PIN_NUM           12
#define CNI_14_PIN_TYPE          B
#define CNI_14_16                1

#define CNI_15_PIN_NUM           11
#define CNI_15_PIN_TYPE          B
#define CNI_15_16                1

#define CNI_16_PIN_NUM           10
#define CNI_16_PIN_TYPE          B
#define CNI_16_16                1

#define CNI_17_PIN_NUM           7
#define CNI_17_PIN_TYPE          C
#define CNI_17_16                2

#define CNI_18_PIN_NUM           6
#define CNI_18_PIN_TYPE          C
#define CNI_18_16                2

#define CNI_19_PIN_NUM           9
#define CNI_19_PIN_TYPE          C
#define CNI_19_16                2

#define CNI_20_PIN_NUM           8
#define CNI_20_PIN_TYPE          C
#define CNI_20_16                2

#define CNI_21_PIN_NUM           9
#define CNI_21_PIN_TYPE          B
#define CNI_21_16                2

#define CNI_22_PIN_NUM           8
#define CNI_22_PIN_TYPE          B
#define CNI_22_16                2

#define CNI_23_PIN_NUM           7
#define CNI_23_PIN_TYPE          B
#define CNI_23_16                2

#define CNI_24_PIN_NUM           6
#define CNI_24_PIN_TYPE          B
#define CNI_24_16                2

#define CNI_25_PIN_NUM           4
#define CNI_25_PIN_TYPE          C
#define CNI_25_16                2

#define CNI_26_PIN_NUM           5
#define CNI_26_PIN_TYPE          C
#define CNI_26_16                2

#define CNI_27_PIN_NUM           5
#define CNI_27_PIN_TYPE          B
#define CNI_27_16                2

#define CNI_28_PIN_NUM           3
#define CNI_28_PIN_TYPE          C
#define CNI_28_16                2

#define CNI_29_PIN_NUM           3
#define CNI_29_PIN_TYPE          A
#define CNI_29_16                2

#define CNI_30_PIN_NUM           2
#define CNI_30_PIN_TYPE          A
#define CNI_30_16                2


#define __CNI_TRIS(n) TRIS_BIT(CNI_##n##_PIN_TYPE, CNI_##n##_PIN_NUM)
#define __CNI_IE_(n16,n) CNEN##n16##bits.CN##n##IE
#define __CNI_IE(n16,n) __CNI_IE_(n16,n)
#define __CNI_PULLUP_(n16,n) CNPU##n16##bits.CN##n##PUE
#define __CNI_PULLUP(n16,n) __CNI_PULLUP_(n16,n)
#define __CNI_NEEDSPULLUP(n) CNI_##n##_NEEDSPU
#define __CNI_PORT(n) PIN_PORT(CNI_##n##_PIN_TYPE, \
                                    CNI_##n##_PIN_NUM)

#define _CNI_TRIS(n) __CNI_TRIS(n)
#define _CNI_IE(n) __CNI_IE(CNI_##n##_16, n)
#define _CNI_PULLUP(n) __CNI_PULLUP(CNI_##n##_16, n)
#define _CNI_NEEDSPULLUP(n) __CNI_NEEDSPULLUP(n)
#define _CNI_PORT(n) __CNI_PORT(n)

#define __CNI_INIT(n) IF(CNI_##n##_ENABLE,      \
    do {                                        \
        _CNI_TRIS(n) = 1;                       \
        _CNI_IE(n) = 1;                         \
        _CNI_PULLUP(n) = _CNI_NEEDSPULLUP(n);   \
    }while(0))

#define _CNI_INIT(n) __CNI_INIT(n)

#define __CNI_OP(op)    \
op(0);  op(1);  op(2);  \
op(3);  op(4);  op(5);  \
op(6);  op(7);  op(8);  \
op(9);  op(10); op(11); \
op(12); op(13); op(14); \
op(15); op(16); op(17); \
op(18); op(19); op(20); \
op(21); op(22); op(23); \
op(24); op(25); op(26); \
op(27); op(28); op(29); \
op(30)
#define CNI_OP(op) __CNI_OP(op)

#if APP_USE_CN

#define CNI_DECL_PROC_FN_INTERNAL(x) CNI_DECL_PROC_FN(x, t)
CNI_OP(CNI_DECL_PROC_FN_INTERNAL);

#define _CNI_SIGNAL(n) \
    CNI_CALL_PROC_FN(n, _CNI_PORT(n))

void cni_init() {
    CNI_OP(_CNI_INIT);

    IFS1bits.CNIF = 0;
    IPC4bits.CNIP = IPL_CN_INPUT;
    IEC1bits.CNIE = 1;
}

void _ISR_NOPSV _CNInterrupt() {
    int i;

    for (i = 0; i < CNI_THRESHOLD; ++i);
    CNI_OP(_CNI_SIGNAL);
    IFS1bits.CNIF = 0;
}

#else

void cni_init() {
}

#endif
