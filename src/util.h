/* 
 * File:   util.h
 * Author: Morphe
 *
 * Created on 26 April 2012 ?., 10:06
 */

#ifndef UTIL_H
#define	UTIL_H

#include "system.h"

#define abs_fast(a) ((a)>=0?(a):-(a))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

unsigned pow_fast(unsigned val, unsigned pow);
int trunc_long(long int i);
long int trunc_ulong(long unsigned i);
_Bool in_range(uint16_t start, uint16_t end, uint16_t val);
int uint32_cmp(uint32_t lsv, uint32_t rsv);

#endif	/* UTIL_H */

