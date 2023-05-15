#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initRle(&chunk->lines);
  initValueArray(&chunk->constants);
//   initExtandArray(&chunk->extendCode);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code =
        GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  // already has capacity
  chunk->code[chunk->count] = byte;
  writeRle(&chunk->lines, line);
  chunk->count++;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeRle(&chunk->lines);
  freeValueArray(&chunk->constants);
//   freeExtendArray(&chunk->extendCode);
  initChunk(chunk);
}

//  the index where the constant was appended,
//  to locate the same constant later.
int addConstants(Chunk *chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

/* writeConstant helper */
void writeChunkLongHelper (Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code =
        GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  // already has capacity
  chunk->code[chunk->count] = byte;
  writeRle(&chunk->lines, line);
  chunk->count++;
}

void writeChunkLong(Chunk* chunk, uint32_t uint24t, int line) {
    writeChunk(chunk, uint24t % (1 << 8), line);
    writeChunkLongHelper(chunk, (uint24t >> 8) % (1 << 8), line);
    writeChunkLongHelper(chunk, (uint24t >> 16) % (1 << 8), line);
}
// define a second OP_CONSTANT_LONG instruction.
// stores the operand as a 24-bit number.
void writeConstant(Chunk *chunk, Value constant, int line) {
//   if (chunk->capacity < chunk->count + 1) {
//     int oldCapacity = chunk->capacity;
//     chunk->capacity = GROW_CAPACITY(oldCapacity);
//     chunk->code =
//         GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
//   }

  // store the constant value.
  // writeValueArray(&chunk->constants, constant);
  int constantIndex = addConstants(chunk, constant);
  // store the index of the constant value where in valueArray.
  // writeExtendConstantIndex(chunk, constantIndex);
  if (constantIndex < 256) {
    writeChunk(chunk, constantIndex, line);
  } else {
    // chunk->code[chunk->count] = constantIndex % 256;
    // writeExtendArray(&chunk->extendCode, constantIndex / 256);
    writeChunkLong(chunk, constantIndex, line);
  }
}