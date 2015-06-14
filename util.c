
#include "util.h"

unsigned pow_fast(unsigned val, unsigned pow) {
	unsigned res = 1;
	while (pow>0) {
		if (pow%2 == 0) {
			pow /= 2;
			val *= val;
		} else {
			pow -= 1;
			res *= val;
		}
	}
	return res;
}

int trunc_long(long int i) {
    if(i > 32767)
        return 32767;
    else if(i < -32768)
        return -32768;
    else
        return i;
}

long int trunc_ulong(long unsigned i) {
    if(i > 2147483647UL)
        return 2147483647L;
    else
        return i;
}

_Bool in_range(uint16_t start, uint16_t end, uint16_t val) {
    return start <= val && val <= end;
}
