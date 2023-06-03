#include "chunk.h"
#include "common.h"
#include "value.h"
#include "vm.h"
#include "debug.h"
#include <stdio.h>

VM vm; 

void initVM() {
}

void freeVM() {
}

static InterpretResult run() {
// cause chunk.code stored lineinfo index
// so the constlong can't just decode like ip++
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// #define READ_CONSTANT_LONG() (vm.chunk->constants.values[READ_BYTE() + READ_BYTE() << 8 + READ_BYTE() << 16]  )

  for (;;) {
    uint8_t instruction;
    switch (instruction = READ_BYTE()) { 
      case OP_RETURN: {
        printf("op_return\n");
        return INTERPRET_OK; 
      }
      case OP_CONSTANT: {
        Value constant =  READ_CONSTANT();
        printfValue(constant);
        printf("\n");
        break;
      }
      case OP_CONSTANT_LONG: {
        // Value constant = READ_CONSTANT_LONG();
        // printfValue("constant");
        *vm.ip += 3;
        printf("constant");
        printf("\n");
        break;
      }
      default: printf("out\n"); break;
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  // the IP always points to the next instruction,
  // not the one currently being handled.
  vm.ip = vm.chunk->code;
  return run();
}