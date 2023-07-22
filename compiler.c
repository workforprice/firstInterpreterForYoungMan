#include "compiler.h"
#include "common.h"
#include "scanner.h"

#include <stdio.h>


// clox grammer requires only one token of lookahead 
void compile(const char *source) {
  initScanner(source);

  // temporary code to drive the Scanner.
  int line = -1;
  for (; ; ) {
    Token token = scanToken();
    if (token.line != line) {
      printf("%4d ", token.line);
      line = token.line;
    } else {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type, token.length, token.start);

    if (token.type == TOKEN_EOF)  {
      break;
    }
  }
}