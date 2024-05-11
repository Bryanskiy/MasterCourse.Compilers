#include "function.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <memory>

namespace jade {

void Function::insert(BasicBlock *bb) {
  std::size_t id = 0;
  if (!m_bbs.empty()) {
    id = m_bbs.getLast()->getId() + 1;
  }
  bb->setId(id);

  std::stringstream name;
  name << "bb" << id;
  bb->setName(name.str());

  m_bbs.push_back(bb);
}

std::unique_ptr<Function> Function::copy() const {
  auto ret = std::make_unique<Function>();

  // map: old -> new
  std::unordered_map<Instruction *, Instruction *> instrMapping;
  std::unordered_map<BasicBlock *, BasicBlock *> bbsMapping;

  // shallow copy
  for (auto bb = m_bbs.begin(); bb != m_bbs.end(); ++bb) {
    auto *newBB = new BasicBlock{};

    for (auto instr = bb->begin(); instr != bb->end(); ++instr) {
      auto *newInstr = instr->copy();
      newBB->insert(newInstr);
      instrMapping[&*instr] = newInstr;
    }

    bbsMapping[&*bb] = newBB;
    ret->insert(newBB);
  }

  // replace users/inputs.
  // update bb links inside instrs.
  // build new bb edges.
  for (auto bb = ret->m_bbs.begin(); bb != ret->m_bbs.end(); ++bb) {
    for (auto newInstr = bb->begin(); newInstr != bb->end(); ++newInstr) {
      if (newInstr->getOpcode() == Opcode::GOTO) {
        auto *gotoInstr = static_cast<GotoInstr *>(&*newInstr);
        gotoInstr->setBB(bbsMapping[gotoInstr->getBB()]);

        gotoInstr->getParent()->addSuccessor(gotoInstr->getBB());
      }

      if (newInstr->getOpcode() == Opcode::PHI) {
        auto *phiInstr = static_cast<PhiInstr *>(&*newInstr);
        for (auto opt : *phiInstr) {
          opt.first = bbsMapping[opt.first];
          opt.second = instrMapping[opt.second];
        }

        phiInstr->getParent()->addPhi(phiInstr);
      }

      if (newInstr->getOpcode() == Opcode::IF) {
        auto *ifInstr = static_cast<IfInstr *>(&*newInstr);
        ifInstr->setFalseBB(bbsMapping[ifInstr->getFalseBB()]);
        ifInstr->setTrueBB(bbsMapping[ifInstr->getTrueBB()]);

        ifInstr->getParent()->addSuccessor(ifInstr->getTrueBB());
        ifInstr->getParent()->addSuccessor(ifInstr->getFalseBB());
      }

      for (auto input = newInstr->begin(); input != newInstr->end(); ++input) {
        *input = instrMapping[*input];
        (*input)->addUser(&*newInstr);
      }
    }
  }

  return ret;
}

} // namespace jade
