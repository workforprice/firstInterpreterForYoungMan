#include "chunk.h"
#include "common.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void repl();
static void runFile(const char* filePath);
static char* readFile(const char* filePath);

int main(int argc, const char *argv[]) {
  initVM();

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    exit(64);
  }

  freeVM();
  return 0;
}

static void repl() {
  char line[1024];
  for (; ; ) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

static void runFile(const char* filePath) {
  char* source = readFile(filePath);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) return exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) return exit(70);
}

static char* readFile(const char* filePath) {
  FILE* file = fopen(filePath, "rb");
  // file doesn't exsit, don't have access to file.
  if (file == NULL) {
    fprintf(stderr, "Could not open the file \"%s\".\n", filePath);
    exit(74);
  }

  fseek(file, 0L, SEEK_END); // file end.
  size_t size = ftell(file); // how many bytes of the file.
  rewind(file); // back to file beginning.

  char* buffer = (char*) malloc(size+1); // room for string null byte
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", filePath);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), size, file); // read.
  if (bytesRead < size) {
   fprintf(stderr, "Could not read the file \"%s\".\n", filePath);
   exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}