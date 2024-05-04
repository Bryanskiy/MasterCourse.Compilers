#include "constFolding.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>

namespace jade {

void ConstantFolder::visitInstr(Instruction *instr) {
  if (!canFold(instr)) {
    return;
  }

  switch (instr->getOpcode()) {
  case Opcode::ADD: {
    foldAdd(instr);
    break;
  }
  default:
    break;
  }
}

void ConstantFolder::foldAdd(Instruction *instr) {
  auto *bb = instr->getParent();
  auto builder = InstrBulder(bb);
  auto type = instr->getType();

  auto lhs = instr->input(0);
  auto rhs = instr->input(1);
  assert(lhs->getType() == rhs->getType());

#define REPLACE_ADD(type)                                                      \
  {                                                                            \
    auto res = static_cast<Const##type *>(lhs)->getValue() +                   \
               static_cast<Const##type *>(rhs)->getValue();                    \
    auto *constInstr = new Const##type(res);                                   \
    builder.replace(instr, constInstr);                                        \
    break;                                                                     \
  }

  switch (type) {
  case Type::Tag::I64:
    REPLACE_ADD(I64)
  case Type::Tag::I32:
    REPLACE_ADD(I32)
  case Type::Tag::I16:
    REPLACE_ADD(I16)
  case Type::Tag::I8:
    REPLACE_ADD(I8)
  default:
    break;
  }
} // namespace jade

bool ConstantFolder::canFold(Instruction *instr) {
  for (auto *input : *instr) {
    if (input->getOpcode() != Opcode::CONST) {
      return false;
    }
  }

  return true;
}

} // namespace jade
