#include "memory.h"
#include "value.h"
#include "object.h"
#include <string.h>

void initValueArray(ValueArray* array) {
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}

void writeValueArray(ValueArray* array, Value value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values,
                               oldCapacity, array->capacity);
  }

  array->values[array->count] = value;
  array->count++;
}

void freeValueArray(ValueArray* array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) {
    return false;
  }
  switch (a.type) {
    case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_NIL: return true;
    case VAL_OBJ:{
      ObjString* stringA = AS_STRING(a);
      ObjString* stringB = AS_STRING(b);
      return stringA->length == stringB->length &&
              memcmp(stringA->chars, stringB->chars, 
                    stringA->length) == 0;
    }
    default: return false;
  }
}

void printfValue(Value value) {
  switch (value.type) {
    case VAL_NUMBER:
      printf("%g", AS_NUMBER(value)); break;
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false"); break;
    case VAL_NIL:
      printf("nil"); break;
    case VAL_OBJ: printObject(value); break;
  }
}