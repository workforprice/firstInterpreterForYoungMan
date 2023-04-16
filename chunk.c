#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initRle(&chunk->lines);
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
        writeRle(&chunk->lines, line);
        chunk->lines = GROW_ARRAY(int, chunk->lines,
            oldCapacity, chunk->capacity);

    }

    // already has capacity
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;

    chunk->count ++;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeRle(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

//  the index where the constant was appended,
//  to locate the same constant later.
int addConstants(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}