#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include <stdint.h>
typedef enum {
    OP_RETURN,
} OpCode;

// dynamic array
typedef struct {
    // the number of elements in array
    int capacity;
    // the number of elements in use
    int count;
    // data along with instructions
    uint8_t* code; 
} Chunk;

void initChunk(Chunk* chunk);

void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);
#endif