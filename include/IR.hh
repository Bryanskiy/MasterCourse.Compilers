#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "value.hh"
#include "ilist.hh"

namespace jade {

class InstrBulder;
class Instruction;

class BasicBlock final : public Value, public IListNode<BasicBlock> {
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

  template<typename T, typename ...Args>
  T* create(Args&&... args) {
    auto elem = m_bb->m_instrs.insert(m_inserter, std::forward<Args>(args)...);
    // bump inserter
    m_inserter = elem;
  }
private:
  BasicBlock* m_bb{nullptr};
  Instruction* m_inserter{nullptr};
};

class Instruction : public Value, public IListNode<Instruction> {
public:
  virtual void dump(std::ostream& stream);
  virtual ~Instruction() = 0;

protected:
  IList<Value> m_inputs;

private:
  friend InstrBulder;
};

class IfInstr final : public Instruction {
public:
  void dump(std::ostream& stream) override {
    stream << "IF" << std::endl;
  }

  Value* condition() { return static_cast<Value*>(m_inputs.begin()); }
  BasicBlock* falseBB() { return static_cast<BasicBlock*>(m_inputs.end()); }

  ~IfInstr() {}
private:
};

class BinaryInstr;

} // namespace jade
