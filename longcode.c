#include "longcode.h"
#include "memory.h"

void initExtandArray(ExtendArray* extendArray) {
  extendArray->capacity = 0;
  extendArray->count = 0;
  extendArray->code = NULL;
}


void freeExtendArray(ExtendArray* extendArray) {
  FREE_ARRAY(Code, &extendArray->code, extendArray->capacity);
  initExtandArray(extendArray);
}


// index - the quotient of constant's index in chunk.constants
void writeExtendArray(ExtendArray* extendArray, Code code) {
  // if out of capacity
  if (extendArray->count + 1 > extendArray->capacity) {
    int oldCapacity = extendArray->capacity;
    extendArray->capacity = GROW_CAPACITY(oldCapacity);
    extendArray->code = GROW_ARRAY(Code, &extendArray->code, oldCapacity, extendArray->capacity);
  }

  extendArray->code[extendArray->count] = code;
  extendArray->count++;
}