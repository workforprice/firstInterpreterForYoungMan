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
  if (rle->lines.count == 0 ||
      // (rle->lines.count == 1 &&
      rle->lines.values[rle->lines.count - 1] != line)
    writeDynamicArray(&rle->lines, line);
  else if (rle->lines.count > 1 &&
           rle->lines.values[rle->lines.count - 1] ==
               rle->lines.values[rle->lines.count - 2] &&
           line == rle->lines.values[rle->lines.count - 1]) {
    rle->times.values[rle->times.count - 1]++;
  } else {
    writeDynamicArray(&rle->lines, line);
    writeDynamicArray(&rle->times, 2);
  }
}

int lineInfoHelper (Rle* rle, int offset) {
  int linesIter = 0;
  int timesIter = 0;
  int count = 0;

  while (linesIter < rle->lines.count && offset > count) {
    if (linesIter > 0 && rle->lines.values[linesIter] == rle->lines.values[linesIter - 1]) {
        // same as pre
        count += rle->times.values[timesIter] - 1;
        timesIter++;
    } else {
      count += 1;
    }
         linesIter ++;
  }

  return linesIter;
}
// map offset to line.
// offset is a normal number
int getLine(Rle *rle, int offset) {
  int linesIter = lineInfoHelper(rle, offset);
  return linesIter < rle->lines.count ? rle->lines.values[linesIter]
                                      // : rle->lines.values[rle->lines.count - 1];
                                      : -1;
}

bool isSameLine(Rle* rle, int offset){
  int lineInfoArrayIndex = lineInfoHelper(rle, offset);
  if (lineInfoArrayIndex < 1) {
    return -1;
  }
  else if (lineInfoArrayIndex >= rle->lines.count) {
    return true; // 都在最后一行？？？
  }
  else {
    return (rle->lines.values[lineInfoArrayIndex] == rle->lines.values[lineInfoArrayIndex - 1]) ?
                  true: false;
  } 
}