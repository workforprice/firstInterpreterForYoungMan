#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "value.h"
#include "object.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

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

typedef void (*ParseFn)();
typedef struct {
  // the function to compile a prefix expression starting with a token of that typ
  ParseFn prefix;
  // the function to compile an infix expression whose left operand is followed by a token of that type,
  ParseFn infix;
  // the precedence of an infix expression that uses that token as an operator.
  Precedence precedence;
} ParseRule;
Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk() {
  return compilingChunk;
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
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassembleChunk(currentChunk(), "code");
  }
#endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static uint8_t makeConstant(Value value) {
  int constant = addConstants(currentChunk(), value);
  
  // has support more than unit8_max num. with rle :)
  // if (constant > UINT8_MAX) {
  //   error("too many constants in one chunk.");
  //   return 0;
  // }
  
  return (uint8_t)constant;
}
static void emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}
static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitConstant(NUMBER_VAL(value));
}

// tow quotation marks.
static void string() {
  emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                  parser.previous.length - 2)));
}

static void parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  prefixRule();

  while (precedence <= getRule(parser.current.type)->precedence) {
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule();
  }
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
    case TOKEN_BANG: emitByte(OP_NOT); break;
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default: return;
  }
}
static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expresion.");
}

static void literal() {
  switch (parser.previous.type) {
    case TOKEN_NIL: emitByte(OP_NIL); break;
    case TOKEN_TRUE: emitByte(OP_TRUE); break;
    case TOKEN_FALSE: emitByte(OP_FALSE); break;
    default: return; // unreachable
  }
}

static void binary();

ParseRule rules[] = {
  // [TokenType]        = {prefix,   infix,  infix operator precedence}
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static ParseRule* getRule(TokenType opType) {
  return &rules[opType];
}
static void binary() {
  TokenType operatorType = parser.previous.type;
  ParseRule* rule = getRule(operatorType);
  // left-associateive, using operator precedence as operand's prec
  parsePrecedence((Precedence)(rule->precedence+1));

  switch (operatorType) {
    case TOKEN_PLUS: emitByte(OP_ADD); break;
    case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
    case TOKEN_BANG_EQUAL: emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_EQUAL_EQUAL: emitByte(OP_EQUAL); break;
    case TOKEN_GREATER: emitByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS: emitByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL: emitBytes(OP_GREATER, OP_NOT); break;
    default: return; // Unreachable
  }
}

// clox grammer requires only one token of lookahead 
bool compile(const char *source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

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