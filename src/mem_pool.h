/* 
 * File:   mem_pool.h
 * Author: Morphe
 *
 * Created on 28 ?????? 2012 ?., 15:32
 */

#ifndef MEM_POOL_H
#define	MEM_POOL_H

#include "system.h"

void mempool_init();
void mp_free(void *addr);
void * mp_alloc(size_t size);
void * mp_realloc(void *mem, size_t size);

#endif	/* MEM_POOL_H */

