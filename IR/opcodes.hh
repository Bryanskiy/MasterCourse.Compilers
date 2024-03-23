#pragma once

#include <string_view>
#include <unordered_map>

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

inline std::string_view OpcodeToStr(Opcode opc) {
  static std::unordered_map<Opcode, std::string_view> map = {
      {Opcode::IF, "IF"},       {Opcode::GOTO, "GOTO"}, {Opcode::PHI, "PHI"},
      {Opcode::LE, "LE"},       {Opcode::EQ, "EQ"},     {Opcode::ADD, "ADD"},
      {Opcode::MUL, "MUL"},     {Opcode::SUB, "SUB"},   {Opcode::CAST, "CAST"},
      {Opcode::CONST, "CONST"}, {Opcode::RET, "RET"},
  };

  return map[opc];
}

} // namespace jade
