#include "mem_pool.h"
#include "util.h"

#include <string.h>
#include <stdlib.h>

#define POOL_SIZE 524
#define INVALID_OFFSET 0xFFFF

typedef struct tagPOOLALLOC {
    uint16_t allocStart : 16,
             invalid : 1,
             allocSize : 15;
} _MPoolAlloc;

// XXX address 0x830 is magical in dsPIC33F...
_PERSISTENT __attribute__((address(0x834))) uint16_t _mPoolAllocCount;
_PERSISTENT uint16_t _mPoolAllocEnd;
_PERSISTENT uint8_t _memPool[POOL_SIZE];

void mempool_init() {
    if(reset_is_cold()) {
        _mPoolAllocCount = 0;
        _mPoolAllocEnd = 0;
    }
}

size_t _mp_fix_addr(uint16_t addr) {
    // Align data on word boundary
    return (addr & 1) ? addr + 1 : addr;
}

uint16_t _mp_get_offset(void *addr) {
    if ((void *)_memPool >= addr || (void *)(_memPool+POOL_SIZE) <= addr)
        return INVALID_OFFSET;
    else
        return (uint16_t)addr - (uint16_t) _memPool;
}

inline void * _mp_get_ptr(uint16_t offset) {
    if(offset > POOL_SIZE)
        return 0;
    return _memPool + offset;
}

_MPoolAlloc *_mp_get_mpa(uint16_t i) {
    return (_MPoolAlloc *) (_memPool+(POOL_SIZE-(i+1)*sizeof(_MPoolAlloc)));
}

_MPoolAlloc * _mp_find(uint16_t addr) {
    int i;
    for(i = 0; i < _mPoolAllocCount; ++i) {
        _MPoolAlloc *alloc = _mp_get_mpa(i);
        if(alloc->allocStart == addr)
            return alloc;
    }
    return 0;
}

void _mp_invalidate(uint16_t addr) {
    _MPoolAlloc *alloc = _mp_find(addr);
    if(alloc)
        alloc->invalid = 1;
}

_Bool _mp_allocs_overlap(const _MPoolAlloc *lsv, const _MPoolAlloc *rsv) {
    if(rsv->invalid || lsv->invalid)
        return 0;
    if(in_range(lsv->allocStart, lsv->allocStart + lsv->allocSize - 1, rsv->allocStart) ||
       in_range(lsv->allocStart, lsv->allocStart + lsv->allocSize - 1, rsv->allocStart + rsv->allocSize - 1) ||
       in_range(rsv->allocStart, rsv->allocStart + rsv->allocSize - 1, lsv->allocStart) ||
       in_range(rsv->allocStart, rsv->allocStart + rsv->allocSize - 1, lsv->allocStart + lsv->allocSize - 1))
        return 1;
    else
        return 0;
}

void _mp_free(uint16_t offset) {
    if(offset != INVALID_OFFSET)
        _mp_invalidate(offset);
}

void mp_free(void *addr) {
    return _mp_free(_mp_get_offset(addr));
}


uint16_t _mp_alloc(uint16_t size) {
    _MPoolAlloc newAlloc = {0, 0, size};
    int i, lastTry = 0;
    _MPoolAlloc *alloc, *alloc2 = 0;
    uint16_t prevEnd = 0;
    _Bool flagFound = 0;

    while(!flagFound) {
        /* Find possibly free space */
        for(i = lastTry; i < _mPoolAllocCount; ++i) {
            alloc = _mp_get_mpa(i);
            if(alloc->invalid) {
                alloc2 = alloc; // reuse it later
                newAlloc.allocStart = alloc->allocStart;
                break;
            } else if(alloc->allocStart >= prevEnd + size) {
                newAlloc.allocStart = prevEnd;
                prevEnd = alloc->allocStart + alloc->allocSize;
                break;
            } else
                prevEnd = alloc->allocStart + alloc->allocSize;
        }
        if(i == _mPoolAllocCount) {
            /* No free space between allocations */
            if(_mp_get_ptr(_mPoolAllocEnd + size) > (void *)_mp_get_mpa(i))
                return INVALID_OFFSET; // Not enough space
            newAlloc.allocStart = _mPoolAllocEnd;
            _mPoolAllocEnd += size;
            if(alloc2)
                *alloc2 = newAlloc;
            else {
                *_mp_get_mpa(i) = newAlloc;
                ++_mPoolAllocCount;
            }
            return newAlloc.allocStart;
        }
        lastTry = i + 1;
        if(newAlloc.allocStart + newAlloc.allocSize > _mPoolAllocEnd)
            continue;
        /* Check if this free space is still not allocated */
        for(i = 0; i < _mPoolAllocCount; ++i) {
            alloc = _mp_get_mpa(i);
            if(_mp_allocs_overlap(&newAlloc, alloc))
                break;
        }
        if(i == _mPoolAllocCount) {
            /* We can use this space */
            if(alloc2)
                *alloc2 = newAlloc;
            else {
                if(_mp_get_ptr(_mPoolAllocEnd) >= (void *)_mp_get_mpa(i))
                    return INVALID_OFFSET;
                else {
                    *_mp_get_mpa(i) = newAlloc;
                    ++_mPoolAllocCount;
                }
            }
            return newAlloc.allocStart;
        }
    }
    return INVALID_OFFSET;
}

void * mp_alloc(size_t size) {
    return _mp_get_ptr(_mp_alloc(_mp_fix_addr(size)));
}

uint16_t _mp_realloc(uint16_t mem, uint16_t size) {
    int i;
    _MPoolAlloc *alloc, *oldAlloc = _mp_find(mem), newAlloc;
    if(oldAlloc->allocSize >= size) {
        oldAlloc->allocSize = size;
        return oldAlloc->allocStart;
    }

    newAlloc = *oldAlloc;
    newAlloc.allocSize = size;

    for(i = 0; i < _mPoolAllocCount; ++i) {
        alloc = _mp_get_mpa(i);
        if(alloc != oldAlloc && _mp_allocs_overlap(&newAlloc, alloc))
            break;
    }
    if(i != _mPoolAllocCount) {
        uint16_t newMem = _mp_alloc(size);
        memcpy(_mp_get_ptr(newMem), _mp_get_ptr(mem), oldAlloc->allocSize);
        _mp_free(mem);
        return newMem;
    }
    if(newAlloc.allocStart + newAlloc.allocSize > _mPoolAllocEnd) {
    	if(_mp_get_ptr(newAlloc.allocStart + newAlloc.allocSize) > (void *)_mp_get_mpa(i-1))
    		return INVALID_OFFSET;
    	_mPoolAllocEnd = newAlloc.allocStart + newAlloc.allocSize;
    }
    oldAlloc->allocSize = size;
    return oldAlloc->allocStart;
}

void * mp_realloc(void *mem, size_t size) {
    return _mp_get_ptr(_mp_realloc(_mp_get_offset(mem), _mp_fix_addr(size)));
}
