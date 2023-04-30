/**
 * compressed form of line info
 * source line.
 */
#ifndef clox_rle_h
#define clox_rle_h

#include "common.h"
#include "dynamicArray.h"

// string with escape code by show a character twice
typedef struct {
  DynamicArray lines;
  DynamicArray times;
} Rle;

void initRle(Rle* rle);
void freeRle(Rle* rle);
void writeRle(Rle* rle, int line);
#endif