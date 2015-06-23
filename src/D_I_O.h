/* 
 * File:   D_I_O.h
 * Author: 
 *
 * Created on 02 April 2015
 */

#include "system.h"

void discrete_init();
uint16_t discrete_get_output();
_Bool discrete_get_output_bit(uint16_t idx);
void discrete_set_output(uint16_t output);
void discrete_set_output_bit(_Bool output, uint16_t idx);
uint16_t discrete_get_input();
_Bool discrete_get_input_bit(uint16_t idx);

// Requests to read input values immediately
void discrete_sample();

// Requests to set output values immediately
void discrete_latch();

void discrete_update();
