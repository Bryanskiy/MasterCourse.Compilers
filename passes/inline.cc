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
  auto *callBB = instr->getParent();

  auto splitBB = splitCallerBlock(instr);
  updateInputsDataFlow(instr);
  updateOutputsDataFlow(splitBB, instr);
  moveEntryBB(callBB, instr);
  auto *nextBB = mergeGraphs(instr);

  callBB->removeInstr(instr);
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

  currBB->addSuccessor(newBB);

  return newBB;
}

void Inline::updateOutputsDataFlow(BasicBlock *splitted, Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  auto retsCollector = RetInstrCollector();
  walkFn(&retsCollector, callee);

  auto rets = std::move(retsCollector.rets);
  if (rets.size() == 1) {
    auto *val = static_cast<RetInstr *>(rets[0])->getVal();
    replaceUsers(callInstr, val);
  } else {
    splitted->setInsertPoint(&*splitted->begin());
    auto *phi = splitted->create<PhiInstr>(callInstr->getType());
    for (auto *retInstr : rets) {
      auto *val = static_cast<RetInstr *>(retInstr)->getVal();
      val->addUser(phi);
      phi->addOption(val, retInstr->getParent());
    }

    replaceUsers(callInstr, phi);
  }

  for (auto *retInstr : rets) {
    auto *bb = retInstr->getParent();
    bb->setInsertPoint(retInstr);
    bb->create<GotoInstr>(splitted);
    bb->remove(retInstr);
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

void Inline::moveEntryBB(BasicBlock *callBB, Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  auto calleeStartBB = &*callee->getBasicBlocks().nodes().begin();
  auto *calleeStartInstr = &*calleeStartBB->begin();
  auto *i = &*callee->getBasicBlocks().nodes().begin()->begin();
  while (i) {
    auto *next = i->next();
    calleeStartBB->removeInstr(i);
    callBB->insert(i);
    i = next;
  }

  callee->remove(calleeStartBB);
}

BasicBlock *Inline::mergeGraphs(Instruction *instr) {
  auto *callInstr = static_cast<CallInstr *>(instr);
  auto *callee = callInstr->getCallee();

  // move basic blocks
  auto bbs = callee->getBasicBlocks().nodes();
  auto *startBB = &*bbs.begin();
  auto *bb = startBB;
  while (bb) {
    auto *next = bb->next();
    callee->remove(bb);
    m_caller->insert(bb);
    bb = next;
  }

  return startBB;
}

} // namespace jade
