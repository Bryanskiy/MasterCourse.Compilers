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
  void updateInputsDataFlow(CallInstr *callInstr, Function *callee);
  void updateOutputsDataFlow(BasicBlock *splitted, CallInstr *callInstr,
                             Function *callee);
  void moveEntryBB(BasicBlock *callBB, CallInstr *callInstr, Function *callee);
  BasicBlock *mergeGraphs(CallInstr *callInstr, Function *callee);

private:
  Function *m_caller{nullptr};
};

} // namespace jade
