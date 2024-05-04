#pragma once

#include "IR.hh"
#include "function.hh"
namespace jade {

template <typename VisitorT>
void walkInstr(VisitorT *visitor, Instruction *instr) {}

template <typename VisitorT> void walkBB(VisitorT *visitor, BasicBlock *bb) {
  for (auto instrIt = bb->begin(), end = bb->end(); instrIt != end; ++instrIt) {
    auto instr = &*instrIt;
    visitor->visitInstr(instr);
  }
}

template <typename VisitorT> void walkFn(VisitorT *visitor, Function *fn) {
  auto graph = fn->getBasicBlocks();
  auto rpoIt = RPOIterator<BasicBlocksGraph>::begin(graph);
  auto rpoEnd = RPOIterator<BasicBlocksGraph>::end(graph);
  for (; rpoIt != rpoEnd; ++rpoIt) {
    visitor->visitBB(*rpoIt);
  }
}

struct Visitor {
  virtual void visitFn(Function *fn) { walkFn(this, fn); }
  virtual void visitBB(BasicBlock *bb) { walkBB(this, bb); }
  virtual void visitInstr(Instruction *instr) { walkInstr(this, instr); }
};

} // namespace jade
