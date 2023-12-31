#include <stdint.h>
#include <stdio.h>

#include "RLE.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"

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
  if (offset > 0 && isSameLine(&chunk->lines, offset)) {
    printf("   | ");
  } else {
    printf("%4d ", getLine(&chunk->lines, offset));
  }

  // if (offset > 0) {
  //   printf("%4d ", getLine(&chunk->lines, offset - 1));
  // }
  // printf("%4d ", offset);

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", offset);
    case OP_ADD:
      return simpleInstruction("OP_ADD", offset);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", offset);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", offset);
    case OP_SUBTRACT:
      return simpleInstruction("OP_SUBTRACT", offset);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    case OP_NIL:
      return simpleInstruction("OP_NIL", offset);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", offset);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", offset);
    case OP_NOT:
      return simpleInstruction("OP_NOT", offset);
    case OP_EQUAL:
      return simpleInstruction("OP_EQUAL", offset);
    case OP_LESS:
      return simpleInstruction("OP_LESS", offset);
    case OP_GREATER:
      return simpleInstruction("OP_GREATER", offset);
    default:
      printf("Unknown opcode: %d.\n", instruction);
      return offset + 1;
  }
}
