#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "debug.h"

static int simpleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  // TODO: 直接返回offset + 1，没让实际offset改变。
  return offset + 1;
}

/**
 * valid funcion.
*/
void disassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {
    // instruction can have diffrent size.
    offset = dissasembleInstruction(chunk, offset);
  }
}

int dissasembleInstruction(Chunk* chunk, int offset) {
  printf("%04d ", offset);

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    default:
      printf("Unknown opcode: %d.\n", instruction);
      return offset + 1;
  }
}
