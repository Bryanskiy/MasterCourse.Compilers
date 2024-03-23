#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "ilist.hh"
#include "opcodes.hh"

namespace jade {

class Function;

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

  Type() : m_tag{Tag::None} {}
  Type(Tag tag) : m_tag(tag) {}

  template <Tag tag> static Type create() { return Type{tag}; }

  Tag getType() const { return m_tag; }

private:
  Tag m_tag;
};

class Value {
public:
  Value(std::string &&name, Type type)
      : m_name{std::move(name)}, m_type{type} {}
  Value(Type type) : m_type{type} {}
  Value() = default;

  Type::Tag getType() const { return m_type.getType(); }
  std::string getName() const { return m_name; }
  void setName(std::string &&name) { m_name = std::move(name); }

protected:
  Type m_type{Type::None};
  std::string m_name;
};

class InstrBulder;
class Instruction;
class PhiInstr;

template <typename IT> class Range {
  IT m_begin;
  IT m_end;

public:
  IT begin() { return m_begin; }
  IT end() { return m_end; }

  Range(IT begin, IT end) : m_begin(begin), m_end(end) {}
};

template <typename IT> Range(IT begin, IT end) -> Range<IT>;

class BasicBlock final : public Value, public IListNode {
public:
  BasicBlock() = default;

  auto successors() { return Range(m_succs.begin(), m_succs.end()); }
  auto predecessors() { return Range(m_preds.begin(), m_preds.end()); }
  auto phis() { return Range(m_phis.begin(), m_phis.end()); }

  auto collectSuccessors() const { return m_succs; }
  auto collectPredecessors() const { return m_preds; }

  void removeSuccessor(BasicBlock *bb) {
    auto it = std::find_if(m_succs.begin(), m_succs.end(),
                           [bb](BasicBlock *elem) { return bb == elem; });
    m_succs.erase(it);
    bb->removePredecessor(this);
  }

  auto terminator() const { return m_instrs.getLast(); }

  auto begin() const { return m_instrs.begin(); }
  auto end() const { return m_instrs.end(); }

  auto rbegin() const { return std::reverse_iterator{end()}; }
  auto rend() const { return std::reverse_iterator(begin()); }

  void addSuccessor(BasicBlock *succs) {
    succs->addPredecessor(this);
    m_succs.push_back(succs);
  }
  void addPhi(PhiInstr *instr) { m_phis.push_back(instr); }

  void setId(std::size_t id) { m_id = id; }
  std::size_t getId() { return m_id; }

  bool empty() {
    if (m_instrs.empty()) {
      return true;
    }
    return false;
  }

  void inverseCondition();

private:
  void addPredecessor(BasicBlock *pred) { m_preds.push_back(pred); }
  void removePredecessor(BasicBlock *bb) {
    auto it = std::find_if(m_preds.begin(), m_preds.end(),
                           [bb](BasicBlock *elem) { return bb == elem; });
    m_preds.erase(it);
  }

private:
  friend InstrBulder;

  IList<Instruction> m_instrs;
  std::vector<BasicBlock *> m_preds;
  std::vector<BasicBlock *> m_succs;
  std::vector<PhiInstr *> m_phis;
  Function *m_function{nullptr};

  std::size_t m_id;
};

class InstrBulder final {
public:
  using iterator = IListIterator<Instruction>;

  InstrBulder(BasicBlock *bb) : m_bb{bb} { m_inserter = m_bb->m_instrs.end(); }

  void setInsertPoint(Instruction *inserter) {
    m_inserter = iterator(inserter);
  }

  template <typename T, typename... Args> T *create(Args &&...args);

private:
  BasicBlock *m_bb{nullptr};
  iterator m_inserter{nullptr};
};

class Instruction : public Value, public IListNode {
public:
  Instruction() = default;
  Instruction(Type type) : Value{type} {}
  Instruction(Type type, BasicBlock *bb) : Value{type}, m_bb(bb) {}
  Instruction(std::string &&name, Type type, BasicBlock *bb)
      : Value{std::move(name), type}, m_bb(bb) {}
  virtual ~Instruction() = default;

  BasicBlock *getParent() const { return m_bb; }
  Opcode getOpcode() const { return m_op; }

  void setId(std::size_t id) { m_id = id; }
  std::size_t getId() { return m_id; }

  void setParent(BasicBlock *bb) { m_bb = bb; }
  virtual void dump(std::ostream &stream) = 0;

  friend InstrBulder;

  auto begin() const { return m_inputs.begin(); }
  auto end() const { return m_inputs.end(); }

  void dumpRef(std::ostream &stream) {
    stream << this->getType() << " " << this->getName();
  }

protected:
  Opcode m_op;
  std::vector<Instruction *> m_inputs;

private:
  BasicBlock *m_bb{nullptr};
  std::size_t m_id;
};

class IfInstr final : public Instruction {
public:
  IfInstr() { m_op = Opcode::IF; }

  IfInstr(Instruction *cond, BasicBlock *false_, BasicBlock *true_)
      : IfInstr() {
    m_inputs.push_back(cond);
    m_false_bb = false_;
    m_true_bb = true_;
  }

  IfInstr(Instruction *cond, BasicBlock *false_, BasicBlock *true_,
          std::string &&name)
      : IfInstr(cond, false_, true_) {
    setName(std::move(name));
  }

  void dump(std::ostream &stream) override {
    stream << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " T:" << m_true_bb->getName() << " ";
    stream << "F:" << m_false_bb->getName() << " ";
    stream << std::endl;
  }

  Value *getCondition() { return m_inputs[0]; }
  BasicBlock *getFalseBB() const { return m_false_bb; }
  BasicBlock *getTrueBB() const { return m_true_bb; }

  void setFalseBB(BasicBlock *bb) { m_false_bb = bb; }
  void setTrueBB(BasicBlock *bb) { m_true_bb = bb; }

private:
  friend InstrBulder;

  BasicBlock *m_false_bb{nullptr};
  BasicBlock *m_true_bb{nullptr};
};

class GotoInstr final : public Instruction {
public:
  GotoInstr() { m_op = Opcode::GOTO; }

  GotoInstr(BasicBlock *bb) : GotoInstr() { m_bb = bb; }
  GotoInstr(BasicBlock *bb, std::string &&name) : GotoInstr(bb) {
    setName(std::move(name));
  }

  BasicBlock *getBB() const { return m_bb; }
  void dump(std::ostream &stream) override {
    stream << OpcodeToStr(m_op) << " ";
    stream << m_bb->getName();
    stream << std::endl;
  }

private:
  friend InstrBulder;
  BasicBlock *m_bb{nullptr};
};

class RetInstr final : public Instruction {
public:
  RetInstr() { m_op = Opcode::RET; }
  RetInstr(Instruction *v) : RetInstr() { m_inputs.push_back(v); }
  RetInstr(Instruction *v, std::string &&name) : RetInstr(v) {
    setName(std::move(name));
  }

  void dump(std::ostream &stream) override {
    stream << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " " << std::endl;
  }
};

class PhiInstr final : public Instruction {
public:
  PhiInstr(Type type) : Instruction{type} { m_op = Opcode::PHI; }
  PhiInstr(Type type, std::string &&name) : PhiInstr(type) {
    setName(std::move(name));
  }

  void addOption(Instruction *instr, BasicBlock *bb) {
    assert(getType() == instr->getType());
    m_args.push_back(std::make_pair(bb, instr));
  }

  auto begin() { return m_args.begin(); }
  auto end() { return m_args.end(); }

  void dump(std::ostream &stream) override {
    stream << OpcodeToStr(m_op) << " ";
    for (auto &&[bb, instr] : m_args) {
      stream << bb->getName() << " ";
      instr->dumpRef(stream);
      stream << ", ";
    }
  }

private:
  std::vector<std::pair<BasicBlock *, Instruction *>> m_args;
};

class BinaryInstr : public Instruction {
public:
  BinaryInstr(Instruction *lhs, Instruction *rhs) {
    assert(lhs->getType() == rhs->getType());
    m_inputs.reserve(2);
    m_inputs.push_back(lhs);
    m_inputs.push_back(rhs);
  }

  BinaryInstr(Instruction *lhs, Instruction *rhs, std::string &&name) {
    setName(std::move(name));
  }
};

class CmpInstr final : public BinaryInstr {
public:
  void dump(std::ostream &stream) override {
    // TODO
  }

  CmpInstr(Instruction *lhs, Instruction *rhs, Opcode kind)
      : BinaryInstr(lhs, rhs) {
    m_type = Type::create<Type::I1>();
    m_op = kind;
  }

  CmpInstr(Instruction *lhs, Instruction *rhs, Opcode kind, std::string &&name)
      : CmpInstr(lhs, rhs, kind) {
    setName(std::move(name));
  }
};

class BinaryOp final : public BinaryInstr {
public:
  BinaryOp(Instruction *lhs, Instruction *rhs, Opcode kind)
      : BinaryInstr(lhs, rhs) {
    m_type = lhs->getType();
    m_op = kind;
  }

  BinaryOp(Instruction *lhs, Instruction *rhs, Opcode kind, std::string &&name)
      : BinaryOp(lhs, rhs, kind) {
    setName(std::move(name));
  }

  void dump(std::ostream &stream) override {
    // TODO
  }
};

class CastInstr final : public Instruction {
public:
  CastInstr() { m_op = Opcode::CAST; }

  CastInstr(Instruction *val, Type type) : CastInstr() {
    m_cast = type;
    m_type = m_cast;
    m_inputs.push_back(val);
  }

  CastInstr(Instruction *val, Type type, std::string &&name)
      : CastInstr(val, type) {
    setName(std::move(name));
  }

  void dump(std::ostream &stream) override {
    // TODO
  }

private:
  Type m_cast;
};

// for simplification constant is an instruction yet:
// v1: i64 = const 1_64;
template <class T> class Constant : public Instruction {};

// TODO: how to map ctype and jade type in better way?
#define CONSTANT(cty, jadety)                                                  \
  template <> class Constant<cty> : public Instruction {                       \
  public:                                                                      \
    Constant(cty val) : Instruction{Type::jadety} {                            \
      m_op = Opcode::CONST;                                                    \
      m_val = val;                                                             \
    }                                                                          \
                                                                               \
    Constant(cty val, std::string &&name) : Constant(val) {                    \
      setName(std::move(name));                                                \
    }                                                                          \
                                                                               \
    void dump(std::ostream &stream) override {}                                \
                                                                               \
    cty getValue() const { return m_val; }                                     \
                                                                               \
  private:                                                                     \
    cty m_val;                                                                 \
  };                                                                           \
                                                                               \
  using Const##jadety = Constant<cty>;

CONSTANT(std::int64_t, I64);
CONSTANT(std::int32_t, I32);
CONSTANT(std::int16_t, I16);
CONSTANT(std::int8_t, I8);

template <typename T, typename... Args> T *InstrBulder::create(Args &&...args) {
  auto *elem = new T(args...);
  elem->setParent(m_bb);

  m_bb->m_instrs.insertBefore(m_inserter, elem);
  if constexpr (std::is_same_v<T, IfInstr>) {
    m_bb->addSuccessor(elem->getFalseBB());
    m_bb->addSuccessor(elem->getTrueBB());
  } else if constexpr (std::is_same_v<T, GotoInstr>) {
    m_bb->addSuccessor(elem->getBB());
  } else if constexpr (std::is_same_v<T, PhiInstr>) {
    m_bb->addPhi(elem);
  }

  return elem;
}

} // namespace jade
