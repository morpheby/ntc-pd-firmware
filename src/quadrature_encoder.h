/* 
 * File:   quadrature_encoder.h
 * Author: evgeny
 *
 * Created on 9 Июль 2015 г., 15:11
 */

#ifndef QUADRATURE_ENCODER_H
#define	QUADRATURE_ENCODER_H

void initQEI();

//get mesured speed in radians per second;
float QEI_getSpeedRadS();

//get mesured speed in rotations per second;
float QEI_getSpeedRotS();

#endif	/* QUADRATURE_ENCODER_H */
