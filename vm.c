#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "compiler.h"
#include "object.h"
#include "value.h"
#include "RLE.h"
#include "memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

VM vm; 

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
  vm.objects = NULL;
}

void freeVM() {
  freeObjects();
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  push(OBJ_VAL(result));
}

static Value peek(int distance) {
  return vm.stackTop[-1 - distance];
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  // TODP: there should be a problem. 
  // cause constants index may over uint8_t long.
  //   size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = getLine(&vm.chunk->lines, vm.ip - vm.chunk->code - 1);
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}
static InterpretResult run() {
// cause chunk.code stored lineinfo index
// so the constlong can't just decode like ip++
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
/* #define BINARY_OP(op) \
  do { \
    double b = pop(); \
    double a = pop(); \
    push(a op b); \
  }while (false) */
// #define READ_CONSTANT_LONG() (vm.chunk->constants.values[READ_BYTE() + READ_BYTE() << 8 + READ_BYTE() << 16]  )

#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)

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
      // case OP_ADD:      BINARY_OP(+); break;
      // case OP_SUBTRACT: BINARY_OP(-); break;
      // case OP_MULTIPLY: BINARY_OP(*); break;
      // case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_ADD:{
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        }
        else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double a = AS_NUMBER(pop());
          double b = AS_NUMBER(pop());
          push(NUMBER_VAL(a + b));
        }
        else {
          runtimeError("Operands must be two numbers or strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
        // BINARY_OP(NUMBER_VAL, +); break;
      }
      case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
      case OP_NEGATE:   
        if (!IS_NUMBER(peek(0))) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }

        push(NUMBER_VAL(-AS_NUMBER(pop(0))));
        break;
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
      case OP_NIL: push(NIL_VAL); break;
      case OP_TRUE: push(BOOL_VAL(true)); break;
      case OP_FALSE: push(BOOL_VAL(false)); break;
      case OP_NOT: push(BOOL_VAL(isFalsey(pop()))); break;
      case OP_EQUAL: {
        Value a = pop();
        Value b = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_LESS: BINARY_OP(BOOL_VAL, <); break;
      case OP_GREATER: BINARY_OP(BOOL_VAL, >); break;
      default: printf("out\n"); break;
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  // compile(source);
  // return INTERPRET_OK;

  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
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