#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

/**
  * create or grow array after
  * know the desire capacity.
*/
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, capacity) \
  reallocate(pointer, sizeof(type) * (capacity), 0)

void * reallocate(void* pointer, size_t oldCount, size_t newCount);
#endif