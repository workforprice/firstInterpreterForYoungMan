#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

// max load factor(count / capacity)
#define TABLE_MAX_LOAD 0.75

typedef struct {
  ObjString* key; // 
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);

bool tableSet(Table* table, ObjString* key, Value value);

#endif