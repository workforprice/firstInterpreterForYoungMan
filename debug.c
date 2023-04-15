#include <stdint.h>
#include <stdio.h>

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
  int constant = chunk->code[offset+1]; // actrualy it is the index of constant
  printf("%-16s %4d '", name, constant);
  printfValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
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
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    default:
      printf("Unknown opcode: %d.\n", instruction);
      return offset + 1;
  }
}