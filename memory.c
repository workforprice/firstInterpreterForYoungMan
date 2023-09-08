#include <stdlib.h>

#include "memory.h"

void * reallocate(void* pointer, size_t oldCount, size_t newCount) {
  if (newCount == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newCount);
  if (result == NULL) {
    exit(1);
  }
  return result;
}


static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_STRING:{
      ObjString * objString = (ObjString*) object;
      FREE_ARRAY(char, objString->chars, objString->length + 1);
      FREE(ObjString, objString);
      break;
    }
  }
}

void freeObjects() {
  Obj* objects = vm.objects;
  while (objects != NULL) {
    Obj* next = objects->next;
    freeObject(objects);
    objects = next;
  }
}

