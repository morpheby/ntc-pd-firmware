/* 
 * File:   messaging.h
 * Author: Morphe
 *
 * Created on 3 ???? 2012 ?., 16:06
 */

#ifndef MESSAGING_H
#define	MESSAGING_H

#include "system.h"


typedef uint8_t _Address;

void messages_init();

/* Checks for new messages and parses them */
void messages_check();

/* Registers another node in the network */
void messages_reg_device();

/* Registers self in the network */
void messages_register();

/* Detaches node from the network */
void messages_unreg();

// NOTE: resID may be null
_Address find_name(const char *name, uint16_t *resID);

// return true if name is unique
_Bool check_name(const char *name);

void switch_control(const char *name, _Bool switchOn);
_Bool switch_status(const char *name);

// NOTE: Returned pointer shall be freed with gc_free()
// NOTE: dataSize may NOT be null
void * get_data(const char *name, size_t *dataSize);

void synchronize();




#endif	/* MESSAGING_H */

