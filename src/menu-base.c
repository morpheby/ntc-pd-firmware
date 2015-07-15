
#include "menu-base.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "app_connector.h"

#if APP_USE_LED_DISP

#define STR_STILL_COUNT_MLT 290
#define STR_SCROLL_COUNT 250
#define STR_SCROLLFAST_COUNT 30

void disp_num(uint8_t dispNum, int num, uint8_t point);
void itoa_s4(char *buf, int val);

struct {
    _Bool   isShareable;
    int     FP;
    int     num;
    uint8_t pt;
} displayState[SECTION_COUNT];

char *dispStr = 0;
size_t dispStrSz = 0;
int dispShowState = 0; // Str position on the shared screen
uint16_t dispCounter = 0;
_Bool dispChngFlag = 1;
int dispAniActive = 0;

void disp_config() {
    int i;
    for(i = 0; i < SECTION_COUNT; ++i) {
        displayState[i].isShareable = 1;
        displayState[i].FP = -1;
        displayState[i].num = 0;
        displayState[i].pt = 0;
    }
}

void disp_draw() {
    int i, j;
    static char display[DISPLAY_COUNT+1];
    static int sharedDispStart = -1, sharedDispEnd = 0;
#if APP_USE_LED_ANIMATIONS
    static _Bool aniInProgress = 0, aniFlag = 0;
#endif
    
//    display[DISPLAY_COUNT] = 0;
    if(dispChngFlag) {
        dispChngFlag = 0;

#if APP_USE_LED_ANIMATIONS
        aniInProgress = 0;
#endif

        for(i = 0; i < SECTION_COUNT; ++i) {
            if(dispStr) {
                if(displayState[i].isShareable) {
                    if(sharedDispStart < 0)
                        sharedDispStart = i * CHARS_IN_SECTION;
                    else
                        sharedDispEnd = (i+1) * CHARS_IN_SECTION;
                    continue;
                }
            }
            itoa_s4(display+i*CHARS_IN_SECTION, displayState[i].num);
            display[i*CHARS_IN_SECTION+3-displayState[i].pt] |= 1 << 7; // dot
        }
    }
    
    if(sharedDispStart < 0) { // No shareable displays
        display_set(display);
        return;
    }

#if APP_USE_LED_ANIMATIONS
    if(dispAniActive) {
        if(!aniInProgress) {
//            dispShowState = -dispAniActive *
//                    (dispAniActive > 0 ? sharedDispEnd - sharedDispStart : dispStrSz);
            dispShowState = -dispAniActive * (sharedDispEnd - sharedDispStart);
            aniInProgress = 1;
            aniFlag = 1;
        }
        if(aniFlag) {
            for(i = (dispAniActive > 0 ? sharedDispStart : sharedDispEnd - 1);
                dispAniActive > 0 ? (i < sharedDispEnd - 1) : (i > sharedDispStart);
                i += dispAniActive) {
                display[i] = display[i + dispAniActive];
            }
            display[i] = ' ';
            aniFlag = 0;
        }
    } else if(aniInProgress) {
        aniInProgress = 0;
        aniFlag = 0;
    }
#endif

    if(dispStr)
        for(i = sharedDispStart, j = dispShowState; i < sharedDispEnd; ++i, ++j)
            if( 0 <= j && j < dispStrSz )
                display[i] = dispStr[j];
#if APP_USE_LED_ANIMATIONS
            else if(!aniInProgress)
                display[i] = ' ';
#endif

    display_set(display);

#if APP_USE_LED_ANIMATIONS
    if(dispAniActive) {
        if(dispShowState != 0) {
            if(dispCounter >= STR_SCROLLFAST_COUNT) {
                dispShowState += dispAniActive;
                dispCounter = 0;
                aniFlag = 1;
            }
            ++dispCounter;
        } else {
            dispCounter = 0;
            dispAniActive = 0;
            aniInProgress = 0;
        }
    } else if(dispStrSz > sharedDispEnd - sharedDispStart &&
              dispStr[dispShowState+sharedDispEnd-sharedDispStart] != 0) {
        // Scroll line
        if((dispShowState == 0 && dispCounter >= STR_STILL_COUNT_MLT*(sharedDispEnd - sharedDispStart)) ||
           (dispShowState != 0 && dispCounter >= STR_SCROLL_COUNT) ) {
            ++dispShowState;
            dispCounter = 0;
        }
        ++dispCounter;
    } else if (dispStr[dispShowState+sharedDispEnd-sharedDispStart] == 0 &&
            dispCounter < STR_STILL_COUNT_MLT*(sharedDispEnd - sharedDispStart)) {
        ++dispCounter;
    } else {
        dispShowState = 0;
        dispCounter = 0;
    }
#endif
}

void itoa_s4(char *buf, int val) {
    if(val > 9999 || val < -999) 
        strcpy(buf, "1---");
    else {
        int i, v = abs_fast(val);
        for(i = 3; i >= 0; --i) {
            buf[i] = v%10 + '0';
            v /= 10;
        }
        if(val < 0)
            buf[0] = '-';
    }
}

void _disp_update() {
    dispShowState = 0;
    dispCounter = 0;
    dispAniActive = 0;
    dispChngFlag = 1;
}

void _disp_update_no_ani_reset() {
    dispChngFlag = 1;
}

void disp_set_fixed(uint8_t dispNum) {
    displayState[dispNum].isShareable = 0;
    _disp_update();
}

void disp_set_shareable(uint8_t dispNum) {
    displayState[dispNum].isShareable = 1;
    _disp_update();
}

void disp_fix_point(uint8_t dispNum, int fp) {
    displayState[dispNum].FP = fp;
    _disp_update();
}

void _disp_puts(const char *str) {
    if(dispStr)
        gc_free(dispStr);
    dispStrSz = strlen(str);
    dispStr = gc_malloc(dispStrSz+1);
    strcpy(dispStr, str);
}

void disp_puts(const char *str) {
    _disp_puts(str);
    _disp_update();
}

void disp_puts_ani(const char *str, int ani) {
//    if(dispNewStr)
//        gc_free(dispNewStr);
//    dispNewStr = gc_malloc(strlen(str)+1);
//    strcpy(dispNewStr, str);
//
//    dispShowState = 0;
//    dispCounter = 0;
//    dispChngFlag = 1;
    _disp_puts(str);
    dispAniActive = ani;
    _disp_update_no_ani_reset();
}

void disp_cls() {
    gc_free(dispStr);
    dispStr = 0;
    dispStrSz = 0;
    _disp_update();
}

void disp_num(uint8_t dispNum, int num, uint8_t point) {
    if(displayState[dispNum].isShareable && dispStr)
        disp_cls();
    displayState[dispNum].num = num;
    displayState[dispNum].pt = point;
    _disp_update();
}

void disp_puti(uint8_t dispNum, int i) {
    if(displayState[dispNum].FP > 0) {
        long int t;
        t = i * pow_fast(10, displayState[dispNum].FP);
        disp_num(dispNum, trunc_long(t), displayState[dispNum].FP);
    } else
        disp_num(dispNum, i, 0);
}

void disp_putsi(uint8_t dispNum, short int si) {
    disp_puti(dispNum, si);
}
void disp_putli(uint8_t dispNum, long int li) {
    disp_puti(dispNum, trunc_long(li));
}
void disp_putu(uint8_t dispNum, unsigned u) {
    disp_puti(dispNum, trunc_long(u));
}
void disp_putsu(uint8_t dispNum, short unsigned su) {
    disp_putu(dispNum, su);
}
void disp_putlu(uint8_t dispNum, long unsigned lu) {
    disp_putu(dispNum, trunc_long(trunc_ulong(lu)));
}
void disp_putf(uint8_t dispNum, float f) {
    uint8_t pt = 0;

    if(displayState[dispNum].FP >= 0) {
        f *= pow_fast(10, displayState[dispNum].FP);
        pt = displayState[dispNum].FP;
    } else {
        while((-100 < f && f < 1000) && pt < 3) {
            f *= 10;
            ++pt;
        }
    }
    disp_num(dispNum, (int) f, pt);
}
void disp_putd(uint8_t dispNum, double d) {
    disp_putf(dispNum, d);
}
void disp_putld(uint8_t dispNum, long double ld) {
    disp_putd(dispNum, ld);
}

#else

void disp_config() {}
void disp_draw() {}

void disp_cls() {}
void disp_set_fixed(uint8_t dispNum) {}
void disp_set_shareable(uint8_t dispNum) {}
void disp_fix_point(uint8_t dispNum, int fp) {}
void disp_puts(const char *str) {}
void disp_puts_ani(const char *str, int ani) {}
void disp_puti(uint8_t dispNum, int i) {}
void disp_putsi(uint8_t dispNum, short int si) {}
void disp_putli(uint8_t dispNum, long int li) {}
void disp_putu(uint8_t dispNum, unsigned u) {}
void disp_putsu(uint8_t dispNum, short unsigned su) {}
void disp_putlu(uint8_t dispNum, long unsigned lu) {}
void disp_putf(uint8_t dispNum, float f) {}
void disp_putd(uint8_t dispNum, double d) {}
void disp_putld(uint8_t dispNum, long double ld) {}

#endif
