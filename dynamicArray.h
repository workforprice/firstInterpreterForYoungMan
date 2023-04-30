// dynamic array store int.
#ifndef clox_dynamicArray_h
#define clox_dynamicArray_h

#include "common.h"

typedef struct {
  int capacity;
  int count;
  int* values;
} DynamicArray;

void initDynamicArray(DynamicArray* array);
void writeDynamicArray(DynamicArray* array, int value);
void freeDynamicArray(DynamicArray* array);

#endif