#include "checksElimination.hh"
#include "IR.hh"
#include "opcodes.hh"

namespace jade {

void CheckElimination::run(Function *fn) {
  auto graph = fn->getBasicBlocks();
  auto builder = DominatorTreeBuilder<BasicBlocksGraph>();
  m_domTree = builder.build(graph);
  visitFn(fn);
}

void CheckElimination::visitInstr(Instruction *instr) {
  switch (instr->getOpcode()) {
  case Opcode::BoundsCheck: {
    boundsCheckElimination(instr);
    break;
  }
  case Opcode::ZeroCheck: {
    zeroCheckElimination(instr);
    break;
  }
  default:
    break;
  }
}

void CheckElimination::zeroCheckElimination(Instruction *instr) {
  auto *input = instr->input(0);
  auto *bb = instr->getParent();
  for (auto user = input->usersBegin(); user != input->usersEnd(); ++user) {
    if ((*user)->getOpcode() == Opcode::ZeroCheck && *user != instr &&
        dominate(m_domTree, *user, instr)) {
      input->removeUser(instr);
      bb->removeInstr(instr);
    }
  }
}
void CheckElimination::boundsCheckElimination(Instruction *instr) {
  auto *input = instr->input(0);
  auto *bound = instr->input(1);
  auto *bb = instr->getParent();

  for (auto user = input->usersBegin(); user != input->usersEnd(); ++user) {
    if ((*user)->getOpcode() == Opcode::BoundsCheck && *user != instr) {
      auto *secondBound = (*user)->input(1);
      if (secondBound == bound && dominate(m_domTree, *user, instr)) {
        input->removeUser(instr);
        bb->removeInstr(instr);
      }
    }
  }
}

} // namespace jade
