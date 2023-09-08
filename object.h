#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj* next;
};

// ObjString "is" an Obj
// fields in order, pointers in Obj type would take ObjString as Obj
// and read it as Obj, to get ObjType.
struct ObjString {
  Obj obj;
  int length;
  char chars[];
};

ObjString* copyString(const char* chars, int length);

void printObject(Value value);
ObjString* takeString(char* chars, int length);

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value);
}
#endif