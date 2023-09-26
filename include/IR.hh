#pragma once

#include <array>
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
    auto elem = m_bb->m_instrs.insert<T>(m_inserter, std::forward<Args>(args)...);
    // bump inserter
    m_inserter = elem;
    return static_cast<T*>(elem);
  }
private:
  BasicBlock* m_bb{nullptr};
  Instruction* m_inserter{nullptr};
};

struct Instruction : public Value, public IListNode<Instruction> {
  virtual void dump(std::ostream& stream) = 0;
  virtual ~Instruction() = default;

  Instruction() = default;
  Instruction(Type type) : Value{type} {}

private:
  friend InstrBulder;
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
  void addOption(Instruction* instr, BasicBlock* bb) {
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

class BinaryInstr final : public Instruction {
public:
  enum Kind {
    LE,
    ADD,
    MUL,
  };

  BinaryInstr(Value* lhs, Value* rhs, Kind kind) : m_kind(kind) {
    m_inputs[0] = lhs;
    m_inputs[1] = rhs;
  }

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  Kind m_kind;
  std::array<Value*, 2> m_inputs;
};

class CastInstr final : public Instruction {
public:
  CastInstr(Value* val, Type type) : m_val{val}, m_type{type} {}

  void dump(std::ostream& stream) override {
    // TODO
  }
private:
  Type m_type;
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


#define CAST(dstTy, src) static_cast<dstTy>(src)
} // namespace jade
