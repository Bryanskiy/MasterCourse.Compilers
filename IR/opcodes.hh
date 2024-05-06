#pragma once

#include <string_view>
#include <unordered_map>

namespace jade {

enum Opcode {
  // cf
  IF = 0,
  GOTO,
  RET,
  PHI,
  // cmp
  LE,
  EQ,
  // logic
  AND,
  // arithm
  ADD,
  MUL,
  SUB,
  DIV,
  NEG,
  // other
  ASHR,
  CAST,
  CONST,
};

inline std::string_view OpcodeToStr(Opcode opc) {
  static std::unordered_map<Opcode, std::string_view> map = {
      {Opcode::IF, "IF"},       {Opcode::GOTO, "GOTO"}, {Opcode::PHI, "PHI"},
      {Opcode::LE, "LE"},       {Opcode::EQ, "EQ"},     {Opcode::ADD, "ADD"},
      {Opcode::MUL, "MUL"},     {Opcode::SUB, "SUB"},   {Opcode::CAST, "CAST"},
      {Opcode::CONST, "CONST"}, {Opcode::RET, "RET"},   {Opcode::AND, "AND"},
      {Opcode::DIV, "DIV"},     {Opcode::NEG, "NEG"},   {Opcode::ASHR, "ASHR"}};

  return map[opc];
}

} // namespace jade
