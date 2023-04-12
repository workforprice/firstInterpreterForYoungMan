#include "chunk.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>

int main(int argc, const char* argv[]) {
    Chunk chunk;
    
    printf("hello chk\n");
    initChunk(&chunk);
    writeChunk(&chunk, OP_RETURN);
    
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    printf("bye chk\n");
    return 0;
}