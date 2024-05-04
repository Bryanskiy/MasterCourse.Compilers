#pragma once

#include "IR.hh"
#include "PM.hh"
#include "Visitor.hh"

namespace jade {

struct DCE : Pass, Visitor {
  void visitBB(BasicBlock *bb) override;
  void run(Function *fn) override { visitFn(fn); }
};

} // namespace jade
