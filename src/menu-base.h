/* 
 * File:   menu-base.h
 * Author: Morphe
 *
 * Created on 25 ?????? 2012 ?., 11:30
 */

#ifndef MENU_BASE_H
#define	MENU_BASE_H

#include "system.h"
#include "board-config.h"
#include "display.h"

#define SECTION_COUNT DISPLAY_COUNT/CHARS_IN_SECTION

/* NOTE: shared displays must be sequential (i.e. 1 0 1 is not allowed) */

void disp_config();
void disp_draw();

void disp_set_off(uint8_t dispNum, _Bool value);
void disp_cls();
void disp_set_fixed(uint8_t dispNum);
void disp_set_shareable(uint8_t dispNum);
void disp_fix_point(uint8_t dispNum, int fp); // -1 means floating point, 0-3 - point position
void disp_puts(const char *str);
void disp_puts_ani(const char *str, int ani);
void disp_puti(uint8_t dispNum, int i);
void disp_putsi(uint8_t dispNum, short int si);
void disp_putli(uint8_t dispNum, long int li);
void disp_putu(uint8_t dispNum, unsigned u);
void disp_putsu(uint8_t dispNum, short unsigned su);
void disp_putlu(uint8_t dispNum, long unsigned lu);
void disp_putf(uint8_t dispNum, float f);
void disp_putd(uint8_t dispNum, double d);
void disp_putld(uint8_t dispNum, long double ld);


#endif	/* MENU_BASE_H */

