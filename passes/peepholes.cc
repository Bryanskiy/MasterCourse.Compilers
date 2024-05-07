#include "peepholes.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>
#include <iostream>
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
  assert(instr->getOpcode() == Opcode::AND);

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
  if (lhs->getOpcode() != Opcode::CONST && rhs->getOpcode() != Opcode::CONST) {
    return;
  }
  auto constInstr = lhs->getOpcode() == Opcode::CONST ? lhs : rhs;
  // And x, 0 -> const 0
  auto constant = loadIntegerConst(constInstr);
  if (constant.has_value() && constant.value() == 0) {
    auto newConstInstr = createIntegerConstant(0, constInstr->getType());
    builder.insert(newConstInstr.get());
    builder.replaceUsers(instr, newConstInstr.release());
    builder.remove(instr);
  }
}

void PeepHoles::processAdd(Instruction *instr) {
  assert(instr->getOpcode() == Opcode::ADD);

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

  if (lhs->getOpcode() != Opcode::CONST && rhs->getOpcode() != Opcode::CONST) {
    return;
  }
  if (lhs->getOpcode() == Opcode::CONST) {
    std::swap(lhs, rhs);
  }
  // Add x, 0 -> x
  auto constant = loadIntegerConst(rhs);
  if (constant.has_value() && constant.value() == 0) {
    builder.replaceUsers(instr, lhs);
    builder.remove(instr);
  }
}

void PeepHoles::processAshr(Instruction *instr) {
  assert(instr->getOpcode() == Opcode::ASHR);

  auto builder = InstrBulder(instr->getParent());
  builder.setInsertPoint(instr);

  auto *lhs = instr->input(0);
  auto *rhs = instr->input(1);

  // v2 = Shl v0, v1
  // v3. AShr v2, v1 -->  v0
  auto *prevInstr = static_cast<Instruction *>(instr->getPrev());
  if (prevInstr && prevInstr->getOpcode() == Opcode::SHL) {
    auto prevSh = prevInstr->input(1);
    auto prevInput = prevInstr->input(0);
    if (prevInstr->getId() == lhs->getId() && rhs->getId() == prevSh->getId()) {
      builder.replaceUsers(instr, prevInput);
      builder.remove(instr);
    }
  }

  // Ashr x, 0 -> x
  if (lhs->getOpcode() != Opcode::CONST && rhs->getOpcode() != Opcode::CONST) {
    return;
  }
  if (lhs->getOpcode() == Opcode::CONST) {
    std::swap(lhs, rhs);
  }
  auto constant = loadIntegerConst(rhs);
  if (constant.has_value() && constant.value() == 0) {
    builder.replaceUsers(instr, lhs);
    builder.remove(instr);
  }
}

} // namespace jade
