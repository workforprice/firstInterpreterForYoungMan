#include "RLE.h"
#include "chunk.h"
#include "memory.h"

void initLengthArray(LengthArray* numbers) {
  numbers->capacity = 0;
  numbers->count = 0;
  numbers->length = NULL;
}

void initLineArray(LineArray* lines) {
  lines->capacity = 0;
  lines->count = 0;
  lines->line= NULL;
}

void initRle(Rle* rle) {
  initLengthArray(&rle->numbers);
  initLineArray(&rle->lines);
}


void freeLineArray(LineArray* lines) {
  FREE_ARRAY(int, lines->line, lines->capacity);
  initLineArray(lines);
}

void freeLengthArray(LengthArray* numbers) {
  FREE_ARRAY(int, numbers->length, numbers->capacity);
  initLengthArray(numbers);
}

void freeRle(Rle* rle) {
  freeLengthArray(&rle->numbers);
  freeLineArray(&rle->lines);
  initRle(rle);
}


void writeRle(Rle* rle, int line) {

}