#pragma once

#include "IR.hh"
#include "PM.hh"
#include "Visitor.hh"

namespace jade {

struct ConstantFolder : Pass, Visitor {
  void visitInstr(Instruction *instr) override;
  bool canFold(Instruction *instr);
  void run(Function *fn) override { visitFn(fn); }
};

} // namespace jade
