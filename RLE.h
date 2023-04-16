/**
 * compressed form of line info
 * source line.
 */
#ifndef clox_rle_h
#define clox_rle_h

#include "common.h"

// help Rle to record length.
typedef struct {
  int capacity;
  int count;
  int* length;
} LengthArray;

// help Rle to record lines
typedef struct {
  int capacity;
  int count;
  int* line;
} LineArray;
// string with escape code by show a character twice
typedef struct {
  LineArray lines;
  LengthArray numbers;
} Rle;

void initRle(Rle* rle);
void initLineArray(LineArray* lines); 
void initLengthArray(LengthArray* numbers);

void freeRle(Rle* rle);
void freeLengthArray(LengthArray* numbers);
void freeLineArray(LineArray* lines);

void writeLine(Rle* rle, int line);
void writeNumber(Rle* rle, );
void writeRle(Rle* rle, int line);
#endif