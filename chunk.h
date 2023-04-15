#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"
#include <stdint.h>
typedef enum {
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

// dynamic array
typedef struct {
    // the number of elements in array
    int capacity;
    // the number of elements in use
    int count;
    // data along with instructions
    // opcodes
    uint8_t* code; 
    // constants
    // constant values
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);

void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);
int addConstants(Chunk* chunk, Value value);
#endif