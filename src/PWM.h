/* 
 * File:   PWM.h
 * Author: evgeny
 *
 * Created on 20 Июль 2015 г., 13:43
 */

#ifndef PWM_H
#define	PWM_H

#include <stdint.h>

void initPWM();
void PWM_on(uint16_t value);
void PWM_off();

#endif	/* PWM_H */
