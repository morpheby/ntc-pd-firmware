
#include "display.h"
#include "string.h"
#include "board-config.h"

#if HAS_LED_DISPLAY

#define SEG_DRV_SDI_PIN_TYPE    B
#define SEG_DRV_SDI_PIN_NUM     14
#define SEG_DRV_CLK_PIN_TYPE    A
#define SEG_DRV_CLK_PIN_NUM     7
#define SEG_DRV_OE_PIN_TYPE     B
#define SEG_DRV_OE_PIN_NUM      15
#define SEG_DRV_LE_PIN_TYPE     A
#define SEG_DRV_LE_PIN_NUM      10

#define SEG_DRV_5V 0

#define DISP_DRV_A_PIN_TYPE     B
#define DISP_DRV_A_PIN_NUM      13
#define DISP_DRV_B_PIN_TYPE     B
#define DISP_DRV_B_PIN_NUM      12
#define DISP_DRV_C_PIN_TYPE     B
#define DISP_DRV_C_PIN_NUM      11
#define DISP_DRV_D_PIN_TYPE     B
#define DISP_DRV_D_PIN_NUM      10

#define _SEG_DRV_TRIS(type) TRIS_BIT(SEG_DRV_##type##_PIN_TYPE, \
                                  SEG_DRV_##type##_PIN_NUM)
#define _SEG_DRV_LATCH(type) PIN_LATCH(SEG_DRV_##type##_PIN_TYPE, \
                                    SEG_DRV_##type##_PIN_NUM)
#define _SEG_DRV_ODC(type) ODC_BIT(SEG_DRV_##type##_PIN_TYPE, \
                                    SEG_DRV_##type##_PIN_NUM)

#define _DISP_DRV_TRIS(type) TRIS_BIT(DISP_DRV_##type##_PIN_TYPE, \
                                  DISP_DRV_##type##_PIN_NUM)
#define _DISP_DRV_LATCH(type) PIN_LATCH(DISP_DRV_##type##_PIN_TYPE, \
                                    DISP_DRV_##type##_PIN_NUM)

#define SEG_DRV_SDI    _SEG_DRV_LATCH(SDI)
#define SEG_DRV_CLK    _SEG_DRV_LATCH(CLK)
#define SEG_DRV_OE     _SEG_DRV_LATCH(OE)
#define SEG_DRV_LE     _SEG_DRV_LATCH(LE)

#define DISP_DRV_A     _DISP_DRV_LATCH(A)
#define DISP_DRV_B     _DISP_DRV_LATCH(B)
#define DISP_DRV_C     _DISP_DRV_LATCH(C)
#define DISP_DRV_D     _DISP_DRV_LATCH(D)

void set_disp_num(uint8_t num);
void disp_lightup(_Bool on);
void set_seg_char(char c);
void _set_disp_direct(uint8_t num);
void _set_segment_pattern(uint8_t pattern);

const uint8_t _dispMap[DISPLAY_COUNT] = {
//0  1  2  3  4  5  6   7   8  9 10  11
  9, 3, 5, 7, 6, 4, 1, 15, 10, 0, 2, 8
};

const uint8_t _segMap[256] = {
    /*  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 1
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x20, 0x00, // 2
       0xDB, 0x82, 0x9D, 0x9E, 0xC6, 0x5E, 0x5F, 0x8A, 0xDF, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 3
       0x00, 0xCF, 0x57, 0x59, 0x97, 0x5D, 0x4D, 0x5B, 0x47, 0x41, 0x92, 0xC7, 0x51, 0xCB, 0x07, 0x17, // 4
       0xCD, 0xCE, 0x05, 0x5E, 0x55, 0xD3, 0x13, 0xD7, 0xC3, 0xD6, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x10, // 5
       0x00, 0xCF, 0x57, 0x59, 0x97, 0x5D, 0x4D, 0x5B, 0x47, 0x41, 0x92, 0xC7, 0x51, 0xCB, 0x07, 0x17, // 6
       0xCD, 0xCE, 0x05, 0x5E, 0x55, 0x13, 0xD3, 0xD7, 0xC3, 0xD6, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x00, // 7
         /* All ASCII chars higher than 0x80 are substituted by equal chars from 0x00-0x7F plus dot */
       0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 9
       0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x20, 0x00, // A
       0xFB, 0xA2, 0xBD, 0xBE, 0xE6, 0x7E, 0x7F, 0xAA, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // B
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // C
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // D
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // E
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // F
    /*  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
};

char dispBuff[24] = "OPQRSTUVWXYZ";

void disp_init() {
    /* Set segment-driver outputs */
    _SEG_DRV_TRIS(SDI) = 0;
    _SEG_DRV_TRIS(CLK) = 0;
    _SEG_DRV_TRIS(OE) = 0;
    _SEG_DRV_TRIS(LE) = 0;
#if SEG_DRV_5V
    _SEG_DRV_ODC(SDI) = 1;
    _SEG_DRV_ODC(CLK) = 1;
    _SEG_DRV_ODC(OE) = 1;
    _SEG_DRV_ODC(LE) = 1;
#endif

    disp_lightup(0);

    /* Set display-driver outputs */
    _DISP_DRV_TRIS(A) = 0;
    _DISP_DRV_TRIS(B) = 0;
    _DISP_DRV_TRIS(C) = 0;
    _DISP_DRV_TRIS(D) = 0;
}

void _set_disp_direct(uint8_t num) {
    DISP_DRV_A = (num & 1) != 0;
    DISP_DRV_B = (num & 2) != 0;
    DISP_DRV_C = (num & 4) != 0;
    DISP_DRV_D = (num & 8) != 0;
}

void set_disp_num(uint8_t num) {
    _set_disp_direct(_dispMap[num]);
}

void disp_lightup(_Bool on) {
    SEG_DRV_OE = !on;
}

void set_seg_char(char c) {
    _set_segment_pattern(_segMap[(uint8_t)c]);
}

void _set_segment_pattern(uint8_t pattern) {
    int i;
    for(i = 0; i < 8; ++i, pattern >>= 1) {
        SEG_DRV_SDI = (pattern & 1);
        SEG_DRV_CLK = 1;
        SEG_DRV_CLK = 0;
    }
    SEG_DRV_SDI = 0;
    SEG_DRV_LE = 1;
    SEG_DRV_LE = 0;
}

void display_update() {
    int i;
    unsigned k;
    _Bool eol = 0;
    for(i = 0; i < DISPLAY_COUNT; ++i) {
        disp_lightup(0);
        set_disp_num(i);

        char c = dispBuff[i];
        if(!c) // EOL reached
            eol = 1;
        
        if(eol)
            set_seg_char(' ');
        else
            set_seg_char(c);
        
        disp_lightup(1);
        for(k = 0; k < 50; ++k);
    }
}

void display_set(const char *str) {
    strncpy(dispBuff, str, 24);
}

/******************************************************************************/
/*                      bit 3
 *                 ==============
 *               //            //
 *              //            //
 *        bit 6//            //bit 7
 *            //            //
 *           //    bit 2   //
 *           ==============
 *         //            //
 *        //            //
 *  bit 0//            //bit 1
 *      //            //
 *     //   bit 4    //
 *     ==============
 *   O                O
 *  unused           bit 5
 * 
 *   _      _  _       _   _  _   _   _
 *  | |  |  _| _| |_| |_  |_   | |_| |_| _
 *  |_|  | |_  _|   |  _| |_|  | |_|  _|   _   .
 *   _       _       _   _   _                     _           _   _
 *  |_| |_  |    _| |_  |_  |   |_| |    | |_| |  | |  _   _  |_| |_|  _
 *  | | |_| |_  |_| |_  |   |_| | | |   _| |_  |_ | | | | |_| |     | |
 *   _                          _
 *  |_  |_ | |     |_| | | |_|  _|
 *   _| |_ |_| |_| |_| | |  _| |_
 *
 */
 /*****************************************************************************/

#else

void disp_init() {}
void display_update() {}
void display_set(const char *str) {}

#endif

