
#include "display.h"
#include "string.h"
#include "board-config.h"

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
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

const uint8_t _segMap[256] = {
    /*  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 1
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x20, 0x00, // 2
       0xDD, 0x82, 0x9D, 0x9E, 0xC6, 0x5E, 0x5F, 0x8A, 0xDF, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 3
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

char dispBuff[24] = "0000STUVWXYZ";

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
    _DISP_DRV_TRIS(B0) = 0;
    _DISP_DRV_TRIS(B1) = 0;
    _DISP_DRV_TRIS(B2) = 0;
    _DISP_DRV_TRIS(B3) = 0;
    _DISP_DRV_TRIS(B4) = 0;
    _DISP_DRV_TRIS(B5) = 0;
    _DISP_DRV_TRIS(B6) = 0;
    _DISP_DRV_TRIS(B7) = 0;
    _DISP_DRV_TRIS(B8) = 0;
    _DISP_DRV_TRIS(B9) = 0;
    _DISP_DRV_TRIS(B10) = 0;
    _DISP_DRV_TRIS(B11) = 0;
}

void _set_disp_direct(uint8_t num) {
    _DISP_DRV_LATCH(B0) = num != 0;
    _DISP_DRV_LATCH(B1) = num != 1;
    _DISP_DRV_LATCH(B2) = num != 2;
    _DISP_DRV_LATCH(B3) = num != 3;
    _DISP_DRV_LATCH(B4) = num != 4;
    _DISP_DRV_LATCH(B5) = num != 5;
    _DISP_DRV_LATCH(B6) = num != 6;
    _DISP_DRV_LATCH(B7) = num != 7;
    _DISP_DRV_LATCH(B8) = num != 8;
    _DISP_DRV_LATCH(B9) = num != 9;
    _DISP_DRV_LATCH(B10) = num != 10;
    _DISP_DRV_LATCH(B11) = num != 11;
}

void set_disp_num(uint8_t num) {
    _set_disp_direct(_dispMap[num]);
}

void disp_lightup(_Bool on) {
    if (!on)
        _set_disp_direct(-1);
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

        char c = dispBuff[i];
        if(!c) // EOL reached
            eol = 1;
        
        if(eol)
            set_seg_char(' ');
        else
            set_seg_char(c);
        
        set_disp_num(i);
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

