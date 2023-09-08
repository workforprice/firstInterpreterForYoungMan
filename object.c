#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

// #ifdef  FLEXIBLE_ARRAY_MEM
ObjString* copyString(const char* chars, int length) {
  ObjString* string = FLEXIBLE_ARRAY_MEM(ObjString, char, length);
  string->obj.type = OBJ_STRING;
  string->obj.next = vm.objects;
  vm.objects = string->obj.next;
  string->length = length;
  
  memcpy(string->chars, chars, length);
  
  return string;
}

// #else
// static Obj* allocateObject(size_t size, ObjType type) {
//   Obj* object = (Obj*)reallocate(NULL, 0, size);
//   object->type = type;
//   object->next = vm.objects;
//   vm.objects = object;
//   return object;
// }

static ObjString* allocateString(char* heapChars, int length) {
  // ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  // string->length = length;
  // string->chars = heapChars;
  ObjString* string = copyString(heapChars, length);
  return string;
}

// ObjString* copyString(const char* chars, int length) {
//   char* heapChars = ALLOCATE(char, length + 1);
//   memcpy(heapChars, chars, length);
//   heapChars[length] = '\0';
//   return allocateString(heapChars, length);
// }
// #endif

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING: printf("%s", AS_CSTRING(value));
    break;
  }
}

ObjString* takeString(char* chars, int length) {
  return allocateString(chars, length);
}