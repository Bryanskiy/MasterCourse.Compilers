#include "inline.hh"
#include "IR.hh"
#include "Visitor.hh"
#include "function.hh"
#include "opcodes.hh"
#include <cassert>
#include <iostream>
#include <ostream>
#include <vector>

namespace jade {

struct RetInstrCollector : Visitor {
  std::vector<Instruction *> rets;

  void visitInstr(Instruction *instr) override {
    if (instr->getOpcode() == Opcode::RET) {
      rets.push_back(instr);
    }
  }
};

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
  updateInputsDataFlow(instr);
  updateOutputsDataFlow(instr);
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

void Inline::updateOutputsDataFlow(Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  auto retsCollector = RetInstrCollector();
  walkFn(&retsCollector, callee);

  auto rets = std::move(retsCollector.rets);
  if (rets.size() == 1) {
  } else {
  }
}

void Inline::updateInputsDataFlow(Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  BasicBlock *fstBB = &*callee->getBasicBlocks().nodes().begin();
  auto params = fstBB->collectParams();

  std::size_t argCount = 0;
  for (auto arg = callInstr->argsBegin(); arg != callInstr->argsEnd();
       ++arg, ++argCount) {
    auto *param = params[argCount];
    InstrBulder(instr->getParent()).replaceUsers(param, *arg);
    InstrBulder(fstBB).remove(param);
  }
}

} // namespace jade
