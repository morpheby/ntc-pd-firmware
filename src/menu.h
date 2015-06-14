/* 
 * File:   menu.h
 * Author: Morphe
 *
 * Created on 26 ?????? 2012 ?., 11:32
 */

#ifndef MENU_H
#define	MENU_H

#include "system.h"
#include "menu-base.h"

typedef enum tagMENUWORKREASON {
    _mWrkTimer,
    _mWrkBtnLeft,
    _mWrkBtnRight,
    _mWrkBtnEnter,
    _mWrkBtnReturn,
    _mWrkReset
} _MWrkCallReason;

void menu_init();
void menu_worker();
void menu_enter();
void menu_return();
void menu_move(_Bool next);


/* Closes active menu. Use for mode-switching items */
void menu_close();

/* Returns to last menu position. For configuration items */
void menu_show();

#endif	/* MENU_H */

