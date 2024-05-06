#include "peepholes.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>
#include <optional>

namespace jade {

void PeepHoles::visitInstr(Instruction *instr) {
  switch (instr->getOpcode()) {
  case Opcode::ADD: {
    processAdd(instr);
    break;
  }
  case Opcode::AND: {
    processAnd(instr);
    break;
  }
  case Opcode::ASHR: {
    processAshr(instr);
  }
  default:
    break;
  }
}

void PeepHoles::processAnd(Instruction *instr) {
  auto builder = InstrBulder(instr->getParent());
  builder.setInsertPoint(instr);

  auto *lhs = instr->input(0);
  auto *rhs = instr->input(1);

  // And x, x -> x
  if (lhs->getId() == rhs->getId()) {
    builder.replaceUsers(instr, lhs);
    builder.remove(instr);
    return;
  }

  // And x, 0 -> const 0
  for (auto input : {lhs, rhs}) {
    auto constantOpt = loadIntegerConst(input);
    if (constantOpt == std::nullopt) {
      continue;
    }
    auto constant = constantOpt.value();
    if (constant == 0) {
      auto constInstr = createIntegerConstant(constant, input->getType());
      builder.insert(constInstr.get());
      builder.replaceUsers(instr, constInstr.release());
      builder.remove(instr);
    }
  }
}

void PeepHoles::processAdd(Instruction *instr) {
  auto builder = InstrBulder(instr->getParent());
  builder.setInsertPoint(instr);

  auto *lhs = instr->input(0);
  auto *rhs = instr->input(1);

  // add V1, V1 -> shl V1, 1
  if (lhs->getId() == rhs->getId()) {
    auto constInstr = createIntegerConstant(1, lhs->getType());
    builder.insert(constInstr.get());
    auto *shl =
        builder.create<BinaryOp>(lhs, constInstr.release(), Opcode::SHL);
    builder.replaceUsers(instr, shl);
    builder.remove(instr);
    return;
  }

  // Add x, 0 -> x
  for (auto input : {lhs, rhs}) {
    auto constantOpt = loadIntegerConst(input);
    if (constantOpt == std::nullopt) {
      continue;
    }
    auto constant = constantOpt.value();
    if (constant == 0) {
      auto *x = lhs->getOpcode() == Opcode::CONST ? rhs : lhs;
      builder.replaceUsers(instr, x);
      builder.remove(instr);
      return;
    }
  }
}

void PeepHoles::processAshr(Instruction *instr) {}

} // namespace jade
