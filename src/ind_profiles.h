/* 
 * File:   ind_profiles.h
 * Author: Евгений Кравченко
 *
 * Created on 22 Июль 2015 г., 8:12
 */

#ifndef IND_PROFILES_H
#define	IND_PROFILES_H

#include "system.h"

void ind_init();
void ind_showValues();
void ind_putf(uint8_t dispNum, float value, uint8_t precision);
void ind_puti(uint8_t dispNum, int value);
void ind_set_off(uint8_t dispNum);

#endif	/* IND_PROFILES_H */

