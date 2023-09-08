#ifndef clox_compiler_h
#define clox_compiler_h

// void compile(const char* source);
#include "vm.h"
#include "value.h"
#include "scanner.h"
#include "object.h"

bool compile(const char* source, Chunk* chunk);

#endif