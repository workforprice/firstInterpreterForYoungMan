#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"

#include <stdio.h>

int main(int argc, const char* argv[]) {
    Chunk chunk;
    
    printf("hello chk\n");
    initChunk(&chunk);
    int constant = addConstants(&chunk, 33.33);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    int constant2 = addConstants(&chunk, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);

    writeChunk(&chunk, OP_RETURN, 222);

    
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    printf("bye chk\n");
    return 0;
}