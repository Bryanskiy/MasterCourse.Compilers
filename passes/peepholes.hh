#pragma once

#include "IR.hh"
#include "PM.hh"
#include "Visitor.hh"

namespace jade {

struct PeepHoles : Pass, Visitor {
  void visitInstr(Instruction *instr) override;
  void run(Function *fn) override { visitFn(fn); }

  void processAnd(Instruction *instr);
  void processAdd(Instruction *instr);
  void processAshr(Instruction *instr);
};

} // namespace jade
