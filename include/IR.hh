#pragma once

#include <memory>
#include <ostream>
#include <vector>
#include <cstdint>

#include "ilist.hh"

namespace jade {

class Type {
public:
  enum Tag {
    I8 = 0,
    I16,
    I32,
    I64,
    I1, // bool
    None,
  };

  Type(Tag tag) : m_tag(tag) {}

private:
  Tag m_tag;
};

class Value {
public:
  Value(Type type) : m_type{type} {}
  Value() = default;
protected:
  Type m_type{Type::None};
};

class InstrBulder;
class Instruction;

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

struct Instruction : public Value, public IListNode<Instruction> {
  virtual void dump(std::ostream& stream);
  virtual ~Instruction() = 0;

  Instruction() = default;
  Instruction(Type type) : Value{type} {}

private:
  friend InstrBulder;
};

class IfInstr final : public Instruction {
public:
  IfInstr(Instruction* cond, BasicBlock* false_, BasicBlock* true_) :
    m_cond{cond}, m_false_bb{false_}, m_true_bb{true_} {}

  void dump(std::ostream& stream) override {
    stream << "IF" << std::endl;
  }

  Instruction* getCondition() { return m_cond; }
  BasicBlock* getFalseBB() { return m_false_bb; }
  BasicBlock* getTrueBB() { return m_true_bb; }

  ~IfInstr() {}
private:
  Instruction* m_cond{nullptr};
  BasicBlock* m_false_bb{nullptr};
  BasicBlock* m_true_bb{nullptr};
};

class BinaryInstr final : public Instruction {
  // TODO
};

// for simplification constant is an instruction yet:
// v0: i32 = 1;
template<class T>
class Constant : public Instruction {};

#define CONSTANT_SPECIALIZATION(type)                                \
template <>                                                          \
class Constant<type> : public Instruction {                          \
public:                                                              \
  Constant(type val) : Instruction{Type::I64}, m_val{val} {}         \
private:                                                             \
  type m_val;                                                        \
}

CONSTANT_SPECIALIZATION(std::int64_t);
CONSTANT_SPECIALIZATION(std::int32_t);
CONSTANT_SPECIALIZATION(std::int16_t);
CONSTANT_SPECIALIZATION(std::int8_t);

} // namespace jade
