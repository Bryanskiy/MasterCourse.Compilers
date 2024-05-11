#pragma once

#include "PM.hh"
#include "Visitor.hh"
#include "domTree.hh"
#include "function.hh"
namespace jade {

struct ChecksElimination final : Pass, Visitor {
  void visitInstr(Instruction *instr) override;
  void run(Function *fn) override;

private:
  DomTree<BasicBlocksGraph> m_domTree;
  void zeroChecksElimination(Instruction *instr);
  void boundsChecksElimination(Instruction *instr);
};

} // namespace jade
