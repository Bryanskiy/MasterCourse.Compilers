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
  auto *lhs = instr->input(0);
  auto *rhs = instr->input(1);
  // x + 0
  // add V1, V1 -> shl V1, 1
}

void PeepHoles::processAshr(Instruction *instr) {}

} // namespace jade
