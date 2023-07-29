#include "compiler.h"
#include "common.h"
#include "scanner.h"
#include <stdio.h>

typedef struct{
  Token current;
  Token previous;
  bool hadError;
  bool panicMode; // clear at synchronization point.
} Parser;

Parser parser;

static void errorAt(Token* token, const char* message) {
  // suppres any other errors
  if (parser.panicMode) return;
  parser.panicMode = true;
  fprintf(stderr, "[line %d] error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end.");
  }
  else if (token->type == TOKEN_ERROR) {
    // do nothing.
  }
  else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}
static void error(const char* message) {
  errorAt(&parser.previous, message);
}
static void errorAtCurrent(const char *message) {
  errorAt(&parser.current, message);
}
static void advance() {
  parser.previous = parser.current;
  for (; ; ) {
    parser.current = scanToken();
    if(parser.current.type != TOKEN_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
 
}

// has an expected token, or report an error
static void consume(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}
// clox grammer requires only one token of lookahead 
bool compile(const char *source, Chunk* chunk) {
  initScanner(source);

  // temporary code to drive the Scanner.
  // int line = -1;
  // for (; ; ) {
  //   Token token = scanToken();
  //   if (token.line != line) {
  //     printf("%4d ", token.line);
  //     line = token.line;
  //   } else {
  //     printf("   | ");
  //   }
  //   printf("%2d '%.*s'\n", token.type, token.length, token.start);

  //   if (token.type == TOKEN_EOF)  {
  //     break;
  //   }
  // }

  // init
  parser.hadError = false;
  parser.panicMode = false;

  // "primes the pump" on scanner
  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  return !parser.hadError;
}