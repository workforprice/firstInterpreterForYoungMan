#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"

#include <stdio.h>

int main(int argc, const char *argv[]) {
  Chunk chunk;

  printf("hello chk\n");
  initChunk(&chunk);

  for (int i = 0; i < 256; i++) {
    writeChunk(&chunk, OP_CONSTANT, i);
    writeConstant(&chunk, i + i, i);
  }

  for (int i = 256; i < 512; i++) {
    writeChunk(&chunk, OP_CONSTANT_LONG, i);
    writeConstant(&chunk, 999, i);
  }
    // writeChunk(&chunk, OP_CONSTANT_LONG, 256);
    // writeConstant(&chunk, 999, 256);

    // writeChunk(&chunk, OP_CONSTANT_LONG, 257);
    // writeConstant(&chunk, 999, 257);

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  printf("bye chk\n");
  return 0;
}