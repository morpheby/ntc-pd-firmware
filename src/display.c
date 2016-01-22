
#include "display.h"
#include "app_connector.h"
#include "string.h"
#include "board-config.h"
#include <libpic30.h>

#if HAS_LED_DISPLAY

#define SEG_DRV_SDI_PIN_TYPE    C
#define SEG_DRV_SDI_PIN_NUM     9
#define SEG_DRV_CLK_PIN_TYPE    C
#define SEG_DRV_CLK_PIN_NUM     8
//#define SEG_DRV_OE_PIN_TYPE     B
//#define SEG_DRV_OE_PIN_NUM      15
#define SEG_DRV_LE_PIN_TYPE     B
#define SEG_DRV_LE_PIN_NUM      9

#define SEG_DRV_5V 0


#define DISP_DRV_B0_PIN_TYPE     B
#define DISP_DRV_B0_PIN_NUM      8
#define DISP_DRV_B1_PIN_TYPE     B
#define DISP_DRV_B1_PIN_NUM      7
#define DISP_DRV_B2_PIN_TYPE     B
#define DISP_DRV_B2_PIN_NUM      6
#define DISP_DRV_B3_PIN_TYPE     B
#define DISP_DRV_B3_PIN_NUM      5
#define DISP_DRV_B4_PIN_TYPE     A
#define DISP_DRV_B4_PIN_NUM      9
#define DISP_DRV_B5_PIN_TYPE     A
#define DISP_DRV_B5_PIN_NUM      4
#define DISP_DRV_B6_PIN_TYPE     B
#define DISP_DRV_B6_PIN_NUM      4
#define DISP_DRV_B7_PIN_TYPE     A
#define DISP_DRV_B7_PIN_NUM      8
#define DISP_DRV_B8_PIN_TYPE     A
#define DISP_DRV_B8_PIN_NUM      7
#define DISP_DRV_B9_PIN_TYPE     A
#define DISP_DRV_B9_PIN_NUM      10
#define DISP_DRV_B10_PIN_TYPE    B
#define DISP_DRV_B10_PIN_NUM     13
#define DISP_DRV_B11_PIN_TYPE    B
#define DISP_DRV_B11_PIN_NUM     12


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

void set_disp_num(uint8_t num);
void disp_lightup(_Bool on);
void set_seg_char(char c);
void _set_disp_direct(uint8_t num);
void _set_segment_pattern(uint8_t pattern);

const uint8_t _dispMap[] = {
//0  1  2  3  4  5  6  7  8  9  10  11
 // 1, 2, 3, 0, 4, 5, 6, 7, 11, 8, 9, 10
  1, 2, 3, 0, 5, 4, 6, 7, 11, 8, 9, 10
};

const uint8_t _segMap[256] = {
    /*  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00,
    0xF6, 0x24, 0xBA, 0x3E, 0x6C, 0x5E, 0xDE, 0x34, 0xFE, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFC, 0xCE, 0xD2, 0xAE, 0xDA, 0xD8, 0xD6, 0xCC, 0xC0, 0x26, 0xEC, 0xC2, 0xF4, 0x8C, 0x8E,
    0xF8, 0x7C, 0x88, 0x5E, 0xCA, 0xE6, 0x86, 0xEE, 0xE4, 0x6E, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0xFC, 0xCE, 0xD2, 0xAE, 0xDA, 0xD8, 0xD6, 0xCC, 0xC0, 0x26, 0xEC, 0xC2, 0xF4, 0x8C, 0x8E,
    0xF8, 0x7C, 0x88, 0x5E, 0xCA, 0x86, 0xE6, 0xEE, 0xE4, 0x6E, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00,
     /* All ASCII chars higher than 0x80 are substituted by equal chars from 0x00-0x7F plus dot */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x01, 0x00,
    0xF7, 0x25, 0xBB, 0x3F, 0x6D, 0x5F, 0xDF, 0x35, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
};

char dispBuff[12] = "------------";

#define A_A 1
#define A_B 0
#define A_C 0
#define B_A 0
#define B_B 1
#define B_C 0
#define C_A 0
#define C_B 0
#define C_C 1
#define __X(x, t) x##_##t
#define X(x, t) __X(x, t)

#define MASK_HELPER(x, t) (IF_ELSE(X(DISP_DRV_##t##_PIN_TYPE, x), 1<<DISP_DRV_##t##_PIN_NUM, 0))
const static long MASKA = 0
    | MASK_HELPER(A, B0) | MASK_HELPER(A, B1) | MASK_HELPER(A, B2)
    | MASK_HELPER(A, B3) | MASK_HELPER(A, B4) | MASK_HELPER(A, B5)
    | MASK_HELPER(A, B6) | MASK_HELPER(A, B7) | MASK_HELPER(A, B8)
    | MASK_HELPER(A, B9) | MASK_HELPER(A, B10)| MASK_HELPER(A, B11);
const static long MASKB = 0
    | MASK_HELPER(B, B0) | MASK_HELPER(B, B1) | MASK_HELPER(B, B2)
    | MASK_HELPER(B, B3) | MASK_HELPER(B, B4) | MASK_HELPER(B, B5)
    | MASK_HELPER(B, B6) | MASK_HELPER(B, B7) | MASK_HELPER(B, B8)
    | MASK_HELPER(B, B9) | MASK_HELPER(B, B10)| MASK_HELPER(B, B11);
const static long MASKC = 0
    | MASK_HELPER(C, B0) | MASK_HELPER(C, B1) | MASK_HELPER(C, B2)
    | MASK_HELPER(C, B3) | MASK_HELPER(C, B4) | MASK_HELPER(C, B5)
    | MASK_HELPER(C, B6) | MASK_HELPER(C, B7) | MASK_HELPER(C, B8)
    | MASK_HELPER(C, B9) | MASK_HELPER(C, B10)| MASK_HELPER(C, B11);

void disp_init() {
    /* Set segment-driver outputs */
    _SEG_DRV_TRIS(SDI) = 0;
    _SEG_DRV_TRIS(CLK) = 0;
//    _SEG_DRV_TRIS(OE) = 0;
    _SEG_DRV_TRIS(LE) = 0;
#if SEG_DRV_5V
    _SEG_DRV_ODC(SDI) = 1;
    _SEG_DRV_ODC(CLK) = 1;
    _SEG_DRV_ODC(OE) = 1;
    _SEG_DRV_ODC(LE) = 1;
#endif

    disp_lightup(0);

    /* Set display-driver outputs */
    TRISA &= ~MASKA;
    TRISB &= ~MASKB;
    TRISC &= ~MASKC;
}

void _set_disp_direct(uint8_t num) {
    LATA |= MASKA;
    LATB |= MASKB;
    LATC |= MASKC;
    switch(num) {
        case 0: _DISP_DRV_LATCH(B0) = 0; break;
        case 1: _DISP_DRV_LATCH(B1) = 0; break;
        case 2: _DISP_DRV_LATCH(B2) = 0; break;
        case 3: _DISP_DRV_LATCH(B3) = 0; break;
        case 4: _DISP_DRV_LATCH(B4) = 0; break;
        case 5: _DISP_DRV_LATCH(B5) = 0; break;
        case 6: _DISP_DRV_LATCH(B6) = 0; break;
        case 7: _DISP_DRV_LATCH(B7) = 0; break;
        case 8: _DISP_DRV_LATCH(B8) = 0; break;
        case 9: _DISP_DRV_LATCH(B9) = 0; break;
        case 10:_DISP_DRV_LATCH(B10)= 0; break;
        case 11:_DISP_DRV_LATCH(B11)= 0; break;
    }
}

void set_disp_num(uint8_t num) {
    _set_disp_direct(_dispMap[num]);
}

void disp_lightup(_Bool on) {
    if (!on) {
        _set_disp_direct(-1);
    }
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

void display_update(_Bool fullFlag) {
    static int i = 0;
    
    set_disp_num(i);
    set_seg_char(dispBuff[i]);
    
    if(++i >= DISPLAY_COUNT) {
        i = 0;
    }
}

void display_set(const char *str) {
    strncpy(dispBuff, str, 24);
}

/******************************************************************************/
/*                      bit 4
 *                 ==============
 *               //            //
 *              //            //
 *        bit 6//            //bit 5
 *            //            //
 *           //    bit 3   //
 *           ==============
 *         //            //
 *        //            //
 *  bit 7//            //bit 2
 *      //            //
 *     //   bit 1    //
 *     ==============
 *   O                O
 *  unused           bit 0
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
void display_update(_Bool f) {}
void display_set(const char *str) {}

#endif

