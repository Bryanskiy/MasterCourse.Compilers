#pragma once

#include <vector>
#include <memory>

#include "ilist.hh"
#include "instruction.hh"

namespace jade {

class InstrBulder;

class BasicBlock final : public IListNode<BasicBlock> {
public:
private:
  friend InstrBulder;

  IList<Instruction> m_instrs;
};

class InstrBulder final {
public:
  InstrBulder(BasicBlock* bb) : m_bb{bb} {}

  void setInsertPoint(Instruction* inserter) {
    m_inserter = inserter;
  }
private:
  BasicBlock* m_bb{nullptr};
  Instruction* m_inserter{nullptr};
};

} // namespace jade
