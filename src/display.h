/* 
 * File:   display.h
 * Author: Morphe
 *
 * Created on 23 ?????? 2012 ?., 6:09
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include "system.h"

#define DISPLAY_COUNT   12
#define CHARS_IN_SECTION 4

void disp_init();

void display_update();

// Accepts 12-char strings
void display_set(const char *str);

#endif	/* DISPLAY_H */

