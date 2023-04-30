#include "RLE.h"
#include "dynamicArray.h"

void initRle(Rle* rle) {
  initDynamicArray(&rle->lines);
  initDynamicArray(&rle->times);
}

void freeRle(Rle* rle) {
  freeDynamicArray(&rle->lines);
  freeDynamicArray(&rle->times);
}

void writeRle(Rle* rle, int line) {
  // line diffrent from the pre.
  if (line != rle->lines.values[rle->lines.count - 1]) {
    writeDynamicArray(&rle->lines, line);
  }
  else {
    // lines info mentainans - escape code
    if (rle->lines.count > 1 && 
          rle->lines.values[rle->lines.count - 1] != 
            rle->lines.values[rle->lines.count - 2]) {
              // don't have escape code.
              // show times 1 -> more
              // escape code - dup the line info
              // and write how many times the line shows up.
              writeDynamicArray(&rle->lines, line);
              writeDynamicArray(&rle->times, 1);
    }

    // same as pre
    // or correct the show tiems of line in escape code
    rle->times.values[rle->times.count] ++;
  }
}