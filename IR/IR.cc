#include "IR.hh"
#include "opcodes.hh"
#include <algorithm>
#include <array>
#include <cassert>
#include <tuple>
#include <unordered_map>

namespace jade {

void BasicBlock::insert(Instruction *instr) {
  instr->setParent(this);
  instr->setId(m_iid);
  ++m_iid;
  m_instrs.insertBefore(m_inserter, instr);
}

void replaceUsers(Instruction *oldInst, Instruction *newInst) {
  std::for_each(oldInst->usersBegin(), oldInst->usersEnd(),
                [oldInst, newInst](Instruction *user) {
                  newInst->addUser(user);
                  user->replaceInput(oldInst, newInst);
                });
}

void BasicBlock::replace(Instruction *oldInst, Instruction *newInst) {
  auto bb = oldInst->getParent();
  m_instrs.insertBefore(iterator{oldInst}, newInst);

  forget(oldInst);
  replaceUsers(oldInst, newInst);

  m_instrs.remove(oldInst);
}

void BasicBlock::forget(Instruction *instr) {
  std::for_each(instr->begin(), instr->end(), [instr](Instruction *input) {
    auto useOld =
        std::find(input->m_users.begin(), input->m_users.end(), instr);
    input->m_users.erase(useOld);
  });
}

void BasicBlock::remove(Instruction *instr) {
  forget(instr);
  std::for_each(
      instr->usersBegin(), instr->usersEnd(),
      [this, instr](Instruction *user) { user->replaceInput(instr, nullptr); });
  m_instrs.remove(instr);
}

void BasicBlock::dump(std::ostream &stream) {
  for (auto instrIt = begin(), endIt = end(); instrIt != endIt; ++instrIt) {
    auto instr = &*instrIt;
    instr->dump(stream);
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

std::optional<std::int64_t> loadIntegerConst(Instruction *instr) {
  if (instr->getOpcode() != Opcode::CONST) {
    return std::nullopt;
  }

  switch (instr->getType()) {
  case Type::Tag::I64:
    return std::optional(static_cast<ConstI64 *>(instr)->getValue());
  case Type::Tag::I32:
    return std::optional(static_cast<ConstI32 *>(instr)->getValue());
  case Type::Tag::I16:
    return std::optional(static_cast<ConstI16 *>(instr)->getValue());
  case Type::Tag::I8:
    return std::optional(static_cast<ConstI8 *>(instr)->getValue());
  default:
    return std::nullopt;
  }

  assert(0);
}

std::unique_ptr<Instruction> createIntegerConstant(std::int64_t val,
                                                   Type type) {
  switch (type.getType()) {
  case Type::Tag::I64:
    return std::make_unique<ConstI64>(val);
  case Type::Tag::I32:
    return std::make_unique<ConstI32>(val);
  case Type::Tag::I16:
    return std::make_unique<ConstI16>(val);
  case Type::Tag::I8:
    return std::make_unique<ConstI8>(val);
  default:
    assert(0);
  }
}

std::vector<Instruction *> BasicBlock::collectParams() const {
  std::vector<Instruction *> ret;
  for (auto instrIt = begin(); instrIt != end(); ++instrIt) {
    auto instr = &*instrIt;
    if (instr->getOpcode() == Opcode::PARAM) {
      ret.push_back(instr);
    } else {
      break;
    }
  }
  return ret;
}

} // namespace jade
