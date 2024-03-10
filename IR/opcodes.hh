#pragma once

namespace jade {

enum Opcode {
  IF = 0,
  GOTO,
  PHI,
  LE,
  EQ,
  ADD,
  MUL,
  SUB,
  CAST,
  CONST,
  RET,
};

} // namespace jade
