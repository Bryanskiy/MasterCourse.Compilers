#include "inline.hh"
#include "IR.hh"
#include "function.hh"
#include "opcodes.hh"
#include <cassert>
#include <iostream>
#include <ostream>

namespace jade {

void Inline::run(Function *fn) {
  m_caller = fn;

  auto graph = fn->getBasicBlocks();
  auto rpoIt = RPOIterator<BasicBlocksGraph>::begin(graph);
  auto rpoEnd = RPOIterator<BasicBlocksGraph>::end(graph);
  for (; rpoIt != rpoEnd; ++rpoIt) {
    auto *bb = *rpoIt;
    for (auto instrIt = bb->begin(), end = bb->end(); instrIt != end;
         ++instrIt) {
      auto instr = &*instrIt;
      if (instr->getOpcode() == Opcode::CALL) {
        inlineCall(instr);
      }
    }
  }
}

void Inline::inlineCall(Instruction *instr) {
  assert(instr->getOpcode() == Opcode::CALL);

  auto splitBB = splitCallerBlock(instr);
}

BasicBlock *Inline::splitCallerBlock(Instruction *instr) {
  assert(instr->getOpcode() == Opcode::CALL);

  auto *currBB = instr->getParent();
  auto *newBB = m_caller->create<BasicBlock>();
  auto builder = InstrBulder(newBB);

  auto *i = instr->next();
  while (i) {
    auto *next = i->next();
    currBB->removeInstr(i);
    builder.insert(i);
    i = next;
  }

  return newBB;
}

void Inline::updateInputsDataFlow(Instruction *instr) {}

} // namespace jade
