/* 
 * File:   flash_store.h
 * Author: Morphe
 *
 * Created on 9 June 2012, 9:46
 */

#ifndef FLASH_STORE_H
#define	FLASH_STORE_H

#include "system.h"

//#define FLASH_NOAUTOPSV 1

#ifdef FLASH_USEEEPROM
#   define _FLASH_ACCESS __eds__
#   define _FLASH_STORE __attribute__((space(eedata)))
#elif defined FLASH_NOAUTOPSV
#   define _FLASH_ACCESS __psv__
#   define _FLASH_STORE __attribute__((space(psv)))
#else /* Suppose auto-psv */
#   define _FLASH_ACCESS
#   define _FLASH_STORE __attribute__((space(auto_psv)))
#endif

/*
 * flash_write_direct(page, offset, value)
 * 
 * write of a value to the variable, specified
 * by page and offset. Variable shall be declared
 * with _FLASH_STORE.
 * 
 * Example:
 *  _FLASH_ACCESS int x _FLASH_STORE;
 *  int main() {
 *     flash_set(FLASH_GETPAGE(&x), FLASH_GETOFFSET(&x), 10);
 *     flash_write();
 *     return 0;
 *  }
 * NOTE:
 *    Flashing operation COMPLETELY locks device for some time. Even software
 *    resets (including MCLR) will not work while flashing is in progress (not
 *    even minding interrupts). Ensure user will not power off the device until
 *    the operation completes.
 */

void flash_write_direct(unsigned char page, unsigned int offset, uint16_t value);

// TBLPAGE and TBLOFFSET instructions support only explicit address
#define FLASH_GETPAGE(ptr) __builtin_tblpage((ptr))
#define FLASH_GETOFFSET(ptr) __builtin_tbloffset((ptr))

#define FLASH_GETAOFFSET(ptr, i) \
    (__builtin_tbloffset((ptr)) + sizeof((ptr)[0])*(i))


#endif	/* FLASH_STORE_H */

