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

// fullFlag = true -- perform full display redraw
// fullFlag = false -- perform single-char update
void display_update(_Bool fullFlag);

// Accepts 12-char strings
void display_set(const char *str);

#endif	/* DISPLAY_H */

