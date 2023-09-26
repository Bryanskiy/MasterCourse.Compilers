#pragma once

#include <array>
#include <cassert>
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

  template<Tag tag>
  static Type create() {
    return Type{tag};
  }

  Tag getType() const { return m_tag; }
private:
  Tag m_tag;
};

class Value {
public:
  Value(Type type) : m_type{type} {}
  Value() = default;

  Type::Tag getType() const { return m_type.getType(); }
protected:
  Type m_type{Type::None};
};

class InstrBulder;
class Instruction;

class BasicBlock final : public IListNode<BasicBlock> {
public:

  auto predsBegin() { return m_preds.begin(); }
  auto predsEnd() { return m_preds.end(); }

  auto succsBegin() { return m_succs.begin(); }
  auto succsEnd() { return m_succs.end(); }

private:
  friend InstrBulder;

  IList<Instruction> m_instrs;
  std::vector<BasicBlock*> m_preds;
  std::vector<BasicBlock*> m_succs;
};

class InstrBulder final {
public:
  InstrBulder(BasicBlock* bb) : m_bb{bb} {}

  void setInsertPoint(Instruction* inserter) {
    m_inserter = inserter;
  }

  template<typename T, typename ...Args>
  T* create(Args&&... args);

private:
  BasicBlock* m_bb{nullptr};
  Instruction* m_inserter{nullptr};
};

class Instruction : public Value, public IListNode<Instruction> {
public:
  Instruction() = default;
  Instruction(Type type) : Value{type} {}
  virtual ~Instruction() = default;

  BasicBlock* getParent() { return m_bb; }
  virtual void dump(std::ostream& stream) = 0;

  friend InstrBulder;
private:
  BasicBlock* m_bb{nullptr};
};

class IfInstr final : public Instruction {
public:
  IfInstr(Value* cond, BasicBlock* false_, BasicBlock* true_) :
    m_cond{cond}, m_false_bb{false_}, m_true_bb{true_} {}

  void dump(std::ostream& stream) override {
    // TODO
  }

  Value* getCondition() { return m_cond; }
  BasicBlock* getFalseBB() { return m_false_bb; }
  BasicBlock* getTrueBB() { return m_true_bb; }

private:
  friend InstrBulder;

  Value* m_cond{nullptr};
  BasicBlock* m_false_bb{nullptr};
  BasicBlock* m_true_bb{nullptr};
};

class GotoInstr final : public Instruction {
public:
  GotoInstr(BasicBlock* bb) : m_bb{bb} {}

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  friend InstrBulder;
  BasicBlock* m_bb{nullptr};
};

class RetInstr final : public Instruction {
public:
  RetInstr(Value* v) : m_v{v} {}

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  Value* m_v;
};

class PhiInstr final : public Instruction {
public:
  PhiInstr(Type type) : Instruction{type} {}

  void addOption(Instruction* instr, BasicBlock* bb) {
    assert(getType() == instr->getType());
    m_instrs.push_back(instr);
    m_bbs.push_back(bb);
  }

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  std::vector<Instruction*> m_instrs;
  std::vector<BasicBlock*> m_bbs;
};

class BinaryInstr : public Instruction {
public:
  enum Kind {
    LE,
    ADD,
    MUL,
  };

  BinaryInstr(Value* lhs, Value* rhs) {
    assert(lhs->getType() == rhs->getType());
    m_inputs[0] = lhs;
    m_inputs[1] = rhs;
  }

private:
  std::array<Value*, 2> m_inputs;
};

class CmpInstr final : public BinaryInstr {
public:
  enum Kind {
    LE,
  };

  void dump(std::ostream& stream) override {
    // TODO
  }

  CmpInstr(Value* lhs, Value* rhs, Kind kind) : BinaryInstr(lhs, rhs), m_kind(kind) {
    m_type = Type::create<Type::I1>();
  }

private:
  Kind m_kind;
};

class BinaryOp final : public BinaryInstr {
public:
  enum Kind {
    ADD,
    MUL,
  };

  BinaryOp(Value* lhs, Value* rhs, Kind kind) : BinaryInstr(lhs, rhs), m_kind(kind) {
    m_type = lhs->getType();
  }

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  Kind m_kind;
};

class CastInstr final : public Instruction {
public:
  CastInstr(Value* val, Type type) : m_val{val}, m_cast{type} {
    m_type = m_cast;
  }

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  Type m_cast;
  Value* m_val;
};

// for simplification constant is an instruction yet:
// v1: i64 = const 1_64;
template<class T>
class Constant : public Instruction {};

// TODO: how to map ctype and jade type in better way?
#define CONSTANT_SPECIALIZATION(cty, jadety)                          \
template <>                                                           \
class Constant<cty> : public Instruction {                            \
public:                                                               \
  Constant(cty val) : Instruction{Type::jadety}, m_val{val} {}        \
                                                                      \
  void dump(std::ostream& stream) override {                          \
                                                                      \
  }                                                                   \
                                                                      \
  cty getValue() const { return m_val; }                              \
private:                                                              \
  cty m_val;                                                          \
};                                                                    \
                                                                      \
using CONST_##jadety = Constant<cty>;

CONSTANT_SPECIALIZATION(std::int64_t, I64);
CONSTANT_SPECIALIZATION(std::int32_t, I32);
CONSTANT_SPECIALIZATION(std::int16_t, I16);
CONSTANT_SPECIALIZATION(std::int8_t, I8);

template<typename T, typename ...Args>
T* InstrBulder::create(Args&&... args) {
  auto inserted = m_bb->m_instrs.insert<T>(m_inserter, std::forward<Args>(args)...);
  inserted->m_bb = m_bb;

  auto elem = static_cast<T*>(inserted);
  // bump inserter
  m_inserter = elem;

  auto linker = [](BasicBlock* lhs, BasicBlock* rhs) {
    lhs->m_succs.push_back(rhs);
    rhs->m_preds.push_back(lhs);
  };

  if constexpr (std::is_same_v<T, IfInstr>) {
    linker(m_bb, elem->m_true_bb);
    linker(m_bb, elem->m_false_bb);
  } else if constexpr (std::is_same_v<T, GotoInstr>) {
    linker(m_bb, elem->m_bb);
  }

  return elem;
}

} // namespace jade
