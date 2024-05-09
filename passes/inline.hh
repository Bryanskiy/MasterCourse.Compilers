#pragma once

#include "IR.hh"
#include "PM.hh"
namespace jade {

class Inline : public Pass {
public:
  void run(Function *fn) override;

private:
  void inlineCall(Instruction *instr);
  BasicBlock *splitCallerBlock(Instruction *instr);
  void updateInputsDataFlow(Instruction *instr);
  void updateReturn();

private:
  Function *m_caller{nullptr};
};

} // namespace jade
