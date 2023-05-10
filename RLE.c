#include "RLE.h"
#include "dynamicArray.h"

void initRle(Rle *rle) {
  initDynamicArray(&rle->lines);
  initDynamicArray(&rle->times);
}

void freeRle(Rle *rle) {
  freeDynamicArray(&rle->lines);
  freeDynamicArray(&rle->times);
}

void writeRle(Rle *rle, int line) {
  // line diffrent from the pre.
  if (rle->lines.count == 0 ||
      line != rle->lines.values[rle->lines.count - 1]) {
    writeDynamicArray(&rle->lines, line);
  } else if ((rle->lines.count > 1 &&
             rle->lines.values[rle->lines.count - 1] !=
                 rle->lines.values[rle->lines.count - 2])
          || (rle->lines.count = 1 && rle->lines.values[0] == line)) {
    // lines info mentainans - escape code
    // don't have escape code.
    // show times 1 -> 2
    // escape code - dup the line info
    // and write how many times the line shows up.
    writeDynamicArray(&rle->lines, line);
    writeDynamicArray(&rle->times, 2);
  } else {
    // same as pre
    // or correct the show tiems of line in escape code
    rle->times.values[rle->times.count - 1]++;
  }
}

// map offset to line.
// offset is a normal number
int getLine(Rle *rle, int offset) {
  int linesIter = 0;
  int timesIter = 0;
  int count = 0;

  for (; linesIter < rle->lines.count; linesIter++) {
    if (linesIter > 0) {
      if (rle->lines.values[linesIter] != rle->lines.values[linesIter - 1]) {
        // diff the pre
        count += 1;
      } else {
        // same as pre
        count += rle->times.values[timesIter];
        timesIter++;
      }

    } else {
      count += 1;
    }

    if (count > offset) {
      break;
    }
  }

  return linesIter < rle->lines.count ? rle->lines.values[linesIter]
                                      : rle->lines.values[rle->lines.count - 1];
}