#include <stdint.h>
#include <stdio.h>

#include "RLE.h"
#include "chunk.h"
#include "debug.h"

void printfValue(Value value) {
  printf("%g", value);
}
static int simpleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  // TODO: 直接返回offset + 1，没让实际offset改变。
  // the offset of next instruction
  return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
  // offset is where the instruction in the code[],
  // the next code after OP_CONSTANT is constant index.
  // TODO: WHY THERE IS "constant = code[offset + 1]"
  int constant = chunk->code[offset+1]; // the index of constant
  printf("%-16s %4d '", name, constant);
  printfValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

static int constantLongInstruction (const char* name, Chunk* chunk, int offset) {
  int constant = chunk->code[offset + 1] + (chunk->code[offset + 2] << 8) + (chunk->code[offset + 3] << 16);
  printf("%-16s %4d '", name, constant);
  printfValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 4;
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

  // source line
  // decode line info
  // if (offset > 0 && getLine(&chunk->lines, offset) == getLine(&chunk->lines, offset - 1)) {
  //   printf("   | ");
  // } else {
  //   printf("%4d ", getLine(&chunk->lines, offset));
  // }
    printf("%4d ", getLine(&chunk->lines, offset));
  // printf("%4d ", offset);

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    default:
      printf("Unknown opcode: %d.\n", instruction);
      return offset + 1;
  }
}
