// append code with uint16_t space
#ifndef clox_longcode_h
#define clox_longcode_h

#include "common.h"

typedef uint16_t Code;

typedef struct {
  int capacity;
  int count;

  Code* code;
} ExtendArray;

void initExtandArray(ExtendArray* extendArray);
void freeExtendArray(ExtendArray* extendArray);
void writeExtendArray(ExtendArray* extendArray, Code code);
#endif