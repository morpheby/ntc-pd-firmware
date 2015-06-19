/* 
 * File:   display.h
 * Author: Morphe
 *
 * Created on 23 ?????? 2012 ?., 6:09
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include "system.h"
#include "board-config.h"


void disp_init();

void display_update();

// Accepts 12-char strings
void display_set(const char *str);

#endif	/* DISPLAY_H */

