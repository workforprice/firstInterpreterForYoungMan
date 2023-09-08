#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"
#include "object.h"
#include "vm.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define ALLOCATE_OBJ(type, objectType) \
  (type*)allocateObject(sizeof(type), objectType)

#define FLEXIBLE_ARRAY_MEM(structType, memberType, length) \
    (structType*)reallocate(NULL, 0, sizeof(structType) + sizeof(memberType) * (length + 1) )

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

void freeObjects();

#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)
#endif