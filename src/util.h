/* 
 * File:   util.h
 * Author: Morphe
 *
 * Created on 26 April 2012 ?., 10:06
 */

#ifndef UTIL_H
#define	UTIL_H

#include "system.h"

#define abs_fast(a) (a>=0?a:-a)

unsigned pow_fast(unsigned val, unsigned pow);
int trunc_long(long int i);
long int trunc_ulong(long unsigned i);
_Bool in_range(uint16_t start, uint16_t end, uint16_t val);

#endif	/* UTIL_H */

