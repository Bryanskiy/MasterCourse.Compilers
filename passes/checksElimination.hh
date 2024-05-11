#pragma once

#include "PM.hh"
#include "Visitor.hh"
#include "domTree.hh"
#include "function.hh"
namespace jade {

struct CheckElimination final : Pass, Visitor {
  void visitInstr(Instruction *instr) override;
  void run(Function *fn) override;

private:
  DomTree<BasicBlocksGraph> m_domTree;
  void zeroCheckElimination(Instruction *instr);
  void boundsCheckElimination(Instruction *instr);
};

} // namespace jade
