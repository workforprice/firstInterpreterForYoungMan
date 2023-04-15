#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"
#include "value.h"

void printfValue(Value value);

void disassembleChunk(Chunk* chunk, const char* name);
int dissasembleInstruction(Chunk* chunk, int offset);

#endif