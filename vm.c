#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"
#include <stdio.h>

VM vm; 

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
}

void freeVM() {
}

static InterpretResult run() {
// cause chunk.code stored lineinfo index
// so the constlong can't just decode like ip++
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
  do { \
    double b = pop(); \
    double a = pop(); \
    push(a op b); \
  }while (false)
// #define READ_CONSTANT_LONG() (vm.chunk->constants.values[READ_BYTE() + READ_BYTE() << 8 + READ_BYTE() << 16]  )

  for (;;) {
    #ifdef DEBUG_TRACE_EXECUTION
      printf("          ");
      for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        printf("[ ");
        printfValue(*slot);
        printf(" ]");
      }
      printf("\n");
      dissasembleInstruction(vm.chunk, (int) (vm.ip - vm.chunk->code));
    #endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) { 
      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_NEGATE:   push(- pop()); break;
      case OP_RETURN: {
        // printf("op_return\n");
        printfValue(pop());
        printf("\n");
        return INTERPRET_OK; 
      }
      case OP_CONSTANT: {
        Value constant =  READ_CONSTANT();
        printfValue(constant);
        printf("\n");
        push(constant);
        break;
      }
      case OP_CONSTANT_LONG: {
        // Value constant = READ_CONSTANT_LONG();
        // printfValue("constant");
        vm.ip += 3;
        printf("constant");
        printf("\n");
        break;
      }
      default: printf("out\n"); break;
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  compile(source);
  return INTERPRET_OK;
}

// InterpretResult interpret(Chunk* chunk) {
//   vm.chunk = chunk;
//   // the IP always points to the next instruction,
//   // not the one currently being handled.
//   vm.ip = vm.chunk->code;
//   return run();
// }

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop ++;
}

Value pop() {
  vm.stackTop --;
  return *vm.stackTop;
}