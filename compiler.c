#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
  Token current;
  Token previous;
  bool hadError;
  bool panicMode; // clear at synchronization point.
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

Parser parser;
Chunk* compiliingChunk;

static Chunk* currentChunk() {
  return compiliingChunk;
}

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

static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte, parser.previous.line);
}
static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}
static void emitReturn() {
  emitByte(OP_RETURN);
}

static void endCompiler() {
  emitReturn();
}

static uint8_t makeConstant(double value) {
  int constant = addConstants(currentChunk(), value);
  
  // has support more than unit8_max num. with rle :)
  // if (constant > UINT8_MAX) {
  //   error("too many constants in one chunk.");
  //   return 0;
  // }
  
  return (uint8_t)constant;
}
static void emitConstant(double value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}
static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitConstant(value);
}

static void parsePrecedence(Precedence precedence) {

}

static void expression() {
  parsePrecedence(PREC_ASSIGNMENT);
}

// parsing the program in the order it appears in the source code
// rearranging it into the order that execution happens.
static void unary() {
  TokenType operatorType = parser.previous.type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit operator instruction.
  switch (operatorType) {
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default: return;
  }
}
static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expresion.");
}

// clox grammer requires only one token of lookahead 
bool compile(const char *source, Chunk* chunk) {
  initScanner(source);
  compiliingChunk = chunk;

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
  endCompiler(); // wrap compilied things up.
  return !parser.hadError;
}