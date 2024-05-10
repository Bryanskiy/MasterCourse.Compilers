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
  updateOutputsDataFlow(splitBB, instr);
  mergeGraphs(instr);
}

BasicBlock *Inline::splitCallerBlock(Instruction *instr) {
  assert(instr->getOpcode() == Opcode::CALL);

  auto *currBB = instr->getParent();
  auto *newBB = m_caller->create<BasicBlock>();

  auto *i = instr->next();
  while (i) {
    auto *next = i->next();
    currBB->removeInstr(i);
    newBB->insert(i);
    i = next;
  }

  return newBB;
}

void Inline::updateOutputsDataFlow(BasicBlock *splitted, Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  auto retsCollector = RetInstrCollector();
  walkFn(&retsCollector, callee);

  auto rets = std::move(retsCollector.rets);
  if (rets.size() == 1) {
    auto *retInstr = rets[0];
    replaceUsers(callInstr, retInstr);
  } else {
    auto *phi = splitted->create<PhiInstr>(callInstr->getType());
    for (auto *retInstr : rets) {
      retInstr->addUser(phi);
      phi->addOption(retInstr, retInstr->getParent());
    }

    replaceUsers(callInstr, phi);
  }

  for (auto *retInstr : rets) {
    retInstr->getParent()->remove(retInstr);
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
    replaceUsers(param, *arg);
    fstBB->remove(param);
  }
}

void Inline::mergeGraphs(Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  auto bbs = callee->getBasicBlocks().nodes();
  for (auto bbIt = bbs.begin(); bbIt != bbs.end(); ++bbIt) {
    m_caller->create<BasicBlock>(*bbIt);
  }
}

} // namespace jade
