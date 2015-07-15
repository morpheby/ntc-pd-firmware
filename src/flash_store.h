/* 
 * File:   flash_store.h
 * Author: Morphe
 *
 * Created on 9 June 2012, 9:46
 */

#ifndef FLASH_STORE_H
#define	FLASH_STORE_H

#include "system.h"
#include "app_connector.h"

#if APP_USE_RTSP
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
// Use special section defined in the custom linker script to store RTSP-relevant
// functions. Section has to be page-aligned and contain nothing but the code
// marked for this section.
#define SECURE_DECL __attribute__((section(".fixed")))

// TBLPAGE and TBLOFFSET instructions support only explicit address
#define FLASH_GETPAGE(ptr) __builtin_tblpage((ptr))
#define FLASH_GETOFFSET(ptr) __builtin_tbloffset((ptr))

#define FLASH_GETAOFFSET(ptr, i) \
    (__builtin_tbloffset((ptr)) + sizeof((ptr)[0])*(i))

#else

#define _FLASH_ACCESS
#define _FLASH_STORE
#define SECURE_DECL

#define FLASH_GETPAGE(ptr) 0
#define FLASH_GETOFFSET(ptr) 0

#define FLASH_GETAOFFSET(ptr, i) 0

#endif

void flash_init();

/*
 * flash_set(page, offset, value)
 * 
 * Queues write of a value val to the variable, specified
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
 *
 */
void flash_set(unsigned char page, unsigned int offset, uint16_t value);

void flash_set_data(unsigned char page, unsigned int startOffset,
        size_t size, unsigned int *data);

/*
 * flash_write()
 *
 * Flashes all queued writes.
 *
 * Return value:
 *   0   -   Operation succeeded
 *  -1   -   Conflicting flashing operation is pending. Fork and wait, or try
 *           again later
 *  -2   -   Error starting flashing operation. Probably runtime
 *           self-programming was blocked in device configuration
 *  -3   -   Unknown reason. Stored data may be invalid
 *
 * NOTE:
 *    Flashing operation COMPLETELY locks device for some time. Even software
 *    resets (including MCLR) will not work while flashing is in progress (not
 *    even minding interrupts). Ensure user will not power off the device until
 *    the operation completes.
 */
int flash_write();


#endif	/* FLASH_STORE_H */

