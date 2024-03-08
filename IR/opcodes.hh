#pragma once

namespace jade {

enum Opcode {
  IF = 0,
  GOTO,
  PHI,
  LE,
  ADD,
  MUL,
  CAST,
  CONST,
  RET,
};

} // namespace jade
