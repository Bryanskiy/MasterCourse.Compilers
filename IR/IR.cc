#include "IR.hh"
#include <algorithm>
#include <array>
#include <cassert>
#include <tuple>

namespace jade {

void InstrBulder::replace(Instruction *oldInst, Instruction *newInst) {
  auto bb = oldInst->getParent();
  m_bb->m_instrs.insertBefore(iterator{oldInst}, newInst);

  forget(oldInst);
  // rebind users
  std::for_each(oldInst->usersBegin(), oldInst->usersEnd(),
                [this, oldInst, newInst](Instruction *user) {
                  newInst->addUser(user);
                  replaceInput(user, oldInst, newInst);
                });

  m_bb->m_instrs.remove(oldInst);
}

void InstrBulder::forget(Instruction *instr) {
  std::for_each(instr->begin(), instr->end(), [instr](Instruction *input) {
    auto useOld =
        std::find(input->m_users.begin(), input->m_users.end(), instr);
    input->m_users.erase(useOld);
  });
}

void InstrBulder::remove(Instruction *instr) {
  forget(instr);
  std::for_each(
      instr->usersBegin(), instr->usersEnd(),
      [this, instr](Instruction *user) { replaceInput(user, instr, nullptr); });
  m_bb->m_instrs.remove(instr);
}

void BasicBlock::dump(std::ostream &stream) {
  for (auto instrIt = begin(), endIt = end(); instrIt != endIt; ++instrIt) {
    auto instr = &*instrIt;
    instr->dump(stream);
    stream << std::endl;
    stream.flush();
  }
}

void BasicBlock::inverseCondition() {
  assert(m_succs.size() == 2);

  auto lastInstr = m_instrs.getLast();
  auto opcode = lastInstr->getOpcode();
  assert(opcode == Opcode::IF);

  std::swap(m_succs[0], m_succs[1]);

  auto ifInstr = static_cast<IfInstr *>(lastInstr);
  auto tmp = ifInstr->getFalseBB();
  ifInstr->setFalseBB(ifInstr->getTrueBB());
  ifInstr->setTrueBB(tmp);
}

} // namespace jade
