RLE with bug which assume that a line can't maintain more than two lines, but they can.

e.g.
  int constant = addConstants(&chunk, 1.777);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);
  writeChunk(&chunk, OP_NEGATE, 123);

 output:
  0000  123 OP_CONSTANT         0 '1.777'
  0002  124 OP_NEGATE
  0003  124 OP_RETURN

note: line number of OP_NEGATE is wrong.