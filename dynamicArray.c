#include "dynamicArray.h"
#include "memory.h"

void initDynamicArray(DynamicArray* array) {
  array->capacity = 0;
  array->count = 0;
  array->values = NULL;
}

void writeDynamicArray(DynamicArray* array, int value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(int, array->values,
                      oldCapacity, array->capacity);
  }

  // write value
  array->values[array->count] = value;
  array->count ++;
}

void freeDynamicArray(DynamicArray* array) {
  FREE_ARRAY(int, array->values, array->capacity);
  initDynamicArray(array);
}