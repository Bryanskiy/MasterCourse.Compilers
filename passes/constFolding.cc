#include "constFolding.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>

#define EVALUATE_BINARY_OP(lhsInstr, rhsInstr, type, op)                       \
  {                                                                            \
    auto *bb = lhsInstr->getParent();                                          \
    auto lhsVal = static_cast<Constant<type> *>(lhsInstr)->getValue();         \
    auto rhsVal = static_cast<Constant<type> *>(rhsInstr)->getValue();         \
    auto res = op()(lhsVal, rhsVal);                                           \
    auto *constInstr = new Constant<type>(res);                                \
    bb->replace(instr, constInstr);                                            \
  }

#define EVALUATE_BINARY_OP_FOR_EACH_TYPE(lhsInstr, rhsInstr, tag, op)          \
  {                                                                            \
    switch (tag) {                                                             \
    case Type::Tag::I64:                                                       \
      EVALUATE_BINARY_OP(lhsInstr, rhsInstr, std::int64_t, op);                \
      break;                                                                   \
    case Type::Tag::I32:                                                       \
      EVALUATE_BINARY_OP(lhsInstr, rhsInstr, std::int32_t, op);                \
      break;                                                                   \
    case Type::Tag::I16:                                                       \
      EVALUATE_BINARY_OP(lhsInstr, rhsInstr, std::int16_t, op);                \
      break;                                                                   \
    case Type::Tag::I8:                                                        \
      EVALUATE_BINARY_OP(lhsInstr, rhsInstr, std::int8_t, op);                 \
      break;                                                                   \
    case Type::Tag::I1:                                                        \
      EVALUATE_BINARY_OP(lhsInstr, rhsInstr, bool, op);                        \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
  }

#define FOLD_BINARY_OP(instr, op)                                              \
  {                                                                            \
    auto *bb = instr->getParent();                                             \
    auto type = instr->getType();                                              \
                                                                               \
    auto lhs = instr->input(0);                                                \
    auto rhs = instr->input(1);                                                \
    assert(lhs->getType() == rhs->getType());                                  \
                                                                               \
    EVALUATE_BINARY_OP_FOR_EACH_TYPE(lhs, rhs, type, op);                      \
  }

#define EVALUATE_UNARY_OP(instr, type, op)                                     \
  {                                                                            \
                                                                               \
    auto *bb = instr->getParent();                                             \
    auto val = static_cast<Constant<type> *>(instr)->getValue();               \
    auto res = op<type>()(val);                                                \
    auto *constInstr = new Constant<type>(res);                                \
    bb->replace(instr, constInstr);                                            \
  }

#define EVALUATE_UNARY_OP_FOR_EACH_TYPE(instr, tag, op)                        \
  {                                                                            \
    switch (tag) {                                                             \
    case Type::Tag::I64:                                                       \
      EVALUATE_UNARY_OP(instr, std::int64_t, op);                              \
      break;                                                                   \
    case Type::Tag::I32:                                                       \
      EVALUATE_UNARY_OP(instr, std::int32_t, op);                              \
      break;                                                                   \
    case Type::Tag::I16:                                                       \
      EVALUATE_UNARY_OP(instr, std::int16_t, op);                              \
      break;                                                                   \
    case Type::Tag::I8:                                                        \
      EVALUATE_UNARY_OP(instr, std::int8_t, op);                               \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
  }

#define FOLD_UNARY_OP(instr, op)                                               \
  {                                                                            \
    auto type = instr->getType();                                              \
    auto input = instr->input(0);                                              \
    EVALUATE_UNARY_OP_FOR_EACH_TYPE(input, type, op);                          \
  }

namespace jade {

void ConstantFolder::visitInstr(Instruction *instr) {
  if (!canFold(instr)) {
    return;
  }

  switch (instr->getOpcode()) {
  case Opcode::ADD: {
    FOLD_BINARY_OP(instr, std::plus);
    break;
  }
  case Opcode::SUB: {
    FOLD_BINARY_OP(instr, std::minus);
    break;
  }
  case Opcode::MUL: {
    FOLD_BINARY_OP(instr, std::multiplies);
    break;
  }
  case Opcode::DIV: {
    FOLD_BINARY_OP(instr, std::divides);
    break;
  }
  case Opcode::NEG: {
    FOLD_UNARY_OP(instr, std::negate);
    break;
  }
  case Opcode::AND: {
    FOLD_BINARY_OP(instr, std::logical_and);
    break;
  }
  default:
    break;
  }
}

bool ConstantFolder::canFold(Instruction *instr) {
  for (auto *input : *instr) {
    if (input->getOpcode() != Opcode::CONST) {
      return false;
    }
  }

  return true;
}

} // namespace jade
