#pragma once

#include "IR.hh"
#include "PM.hh"
#include "Visitor.hh"

namespace jade {

struct ConstantFolder : Pass, Visitor {
  bool canFold(Instruction *instr);
  void visitInstr(Instruction *instr) override;
  void run(Function *fn) override { visitFn(fn); }

  void foldAdd(Instruction *instr);
};

} // namespace jade
