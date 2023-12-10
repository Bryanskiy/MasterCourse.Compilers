#include "IR.hh"
#include <array>
#include <cassert>
#include <tuple>

namespace jade {

void BasicBlock::inverseCondition() {
    assert(m_succs.size() == 2);

    auto lastInstr = m_instrs.getLast();
    auto opcode = lastInstr->getOpcode();
    assert(opcode == Opcode::IF);

    std::swap(m_succs[0], m_succs[1]);

    auto ifInstr = static_cast<IfInstr*>(lastInstr);
    auto tmp = ifInstr->getFalseBB();
    ifInstr->setFalseBB(ifInstr->getTrueBB());
    ifInstr->setTrueBB(tmp);
}

} // namespace jade
