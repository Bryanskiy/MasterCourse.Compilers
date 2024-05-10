#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <string_view>
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
  std::string_view getName() const {
    static std::unordered_map<Tag, std::string_view> map = {
        {Tag::I1, "i1"},   {Tag::I8, "i8"},   {Tag::I16, "i16"},
        {Tag::I32, "i32"}, {Tag::I64, "i64"}, {Tag::None, "none"}};

    return map[m_tag];
  }

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
  virtual bool is_vreg() const { return false; }

protected:
  Type m_type{Type::None};
  std::string m_name;
};

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
  using iterator = IListIterator<Instruction>;

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

  BasicBlock *next() { return static_cast<BasicBlock *>(getNext()); }
  BasicBlock *prev() { return static_cast<BasicBlock *>(getPrev()); }

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

  void dump(std::ostream &stream);
  void inverseCondition();

  void removeInstr(Instruction *instr) { m_instrs.remove(instr); }

  std::vector<Instruction *> collectParams() const;

  void setInsertPoint(Instruction *inserter) {
    m_inserter = iterator(inserter);
  }
  void replace(Instruction *oldInst, Instruction *newInst);
  void forget(Instruction *instr);
  void remove(Instruction *instr);

  template <typename T, typename... Args> T *create(Args &&...args);
  void insert(Instruction *instr);

private:
  void addPredecessor(BasicBlock *pred) { m_preds.push_back(pred); }
  void removePredecessor(BasicBlock *bb) {
    auto it = std::find_if(m_preds.begin(), m_preds.end(),
                           [bb](BasicBlock *elem) { return bb == elem; });
    m_preds.erase(it);
  }

private:
  IList<Instruction> m_instrs;
  std::vector<BasicBlock *> m_preds;
  std::vector<BasicBlock *> m_succs;
  std::vector<PhiInstr *> m_phis;
  Function *m_function{nullptr};

  iterator m_inserter{nullptr};
  std::size_t m_id{0};
  std::size_t m_iid{0}; // counter for instr id
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
  virtual Instruction *copy() const = 0;

  Instruction *input(std::size_t idx) { return m_inputs[idx]; }
  auto begin() const { return m_inputs.begin(); }
  auto end() const { return m_inputs.end(); }
  auto begin() { return m_inputs.begin(); }
  auto end() { return m_inputs.end(); }
  Instruction *next() { return static_cast<Instruction *>(this->getNext()); }

  void replaceInput(Instruction *oldInstr, Instruction *newInstr) {
    std::replace(m_inputs.begin(), m_inputs.end(), oldInstr, newInstr);
  }

  void addUser(Instruction *instr) { m_users.push_back(instr); }
  void removeUser(Instruction *instr) {
    auto pos = std::find(m_users.begin(), m_users.end(), instr);
    m_users.erase(pos);
  }
  auto usersBegin() const { return m_users.begin(); }
  auto usersEnd() const { return m_users.end(); }

  auto usersBegin() { return m_users.begin(); }
  auto usersEnd() { return m_users.end(); }

  std::size_t numUsers() const { return m_users.size(); }
  void dumpUsers(std::ostream &stream) const {
    for (auto &&elem : m_users) {
      elem->dumpRef(stream);
      stream << " ";
    }
    stream << std::endl;
  }

  void dumpRef(std::ostream &stream) { stream << this->getName(); }
  bool isTerm() const {
    return m_op == Opcode::GOTO || m_op == Opcode::IF || m_op == Opcode::RET;
  }

protected:
  Opcode m_op;
  std::vector<Instruction *> m_inputs;
  std::vector<Instruction *> m_users;

private:
  friend BasicBlock;

  BasicBlock *m_bb{nullptr};
  std::size_t m_id;
};

class ParamInstr : public Instruction {
public:
  ParamInstr(Type type) : Instruction{type} { m_op = Opcode::PARAM; }
  ParamInstr(Type type, std::string &&name) : ParamInstr{type} {
    setName(std::move(name));
  }

  Instruction *copy() const override {
    return new ParamInstr(getType(), getName());
  }

  void dump(std::ostream &stream) override {
    stream << OpcodeToStr(m_op) << " " << getName();
    stream << std::endl;
  }
};

class IfInstr final : public Instruction {
public:
  IfInstr() { m_op = Opcode::IF; }

  IfInstr(Instruction *cond, BasicBlock *false_, BasicBlock *true_)
      : IfInstr() {
    m_inputs.push_back(cond);
    cond->addUser(this);
    m_false_bb = false_;
    m_true_bb = true_;
  }

  IfInstr(Instruction *cond, BasicBlock *false_, BasicBlock *true_,
          std::string &&name)
      : IfInstr(cond, false_, true_) {
    setName(std::move(name));
  }

  Instruction *copy() const override {
    return new IfInstr(m_inputs[0], m_false_bb, m_true_bb, getName());
  }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
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

  Instruction *copy() const override { return new GotoInstr(m_bb, getName()); }

  BasicBlock *getBB() const { return m_bb; }
  void setBB(BasicBlock *bb) { m_bb = bb; }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    stream << m_bb->getName();
    stream << std::endl;
  }

private:
  BasicBlock *m_bb{nullptr};
};

class RetInstr final : public Instruction {
public:
  RetInstr() { m_op = Opcode::RET; }
  RetInstr(Instruction *v) : RetInstr() {
    m_type = v->getType();
    m_inputs.push_back(v);
    v->addUser(this);
  }
  RetInstr(Instruction *v, std::string &&name) : RetInstr(v) {
    setName(std::move(name));
  }

  Instruction *copy() const override {
    return new RetInstr(m_inputs[0], getName());
  }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " " << std::endl;
  }

  Instruction *getVal() const { return m_inputs[0]; }

  bool is_vreg() const override { return true; }
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

  std::pair<BasicBlock *, Instruction *> getOption(std::size_t idx) {
    return m_args[idx];
  }

  auto begin() { return m_args.begin(); }
  auto end() { return m_args.end(); }

  Instruction *copy() const override {
    auto *phi = new PhiInstr(m_type, getName());
    for (auto arg : m_args) {
      phi->addOption(arg.second, arg.first);
    }

    return phi;
  }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    for (auto &&[bb, instr] : m_args) {
      stream << bb->getName() << " ";
      instr->dumpRef(stream);
      stream << ", ";
    }
    stream << std::endl;
  }

  bool is_vreg() const override { return true; }

private:
  std::vector<std::pair<BasicBlock *, Instruction *>> m_args;
};

class UnaryOp final : public Instruction {
public:
  UnaryOp(Instruction *val, Opcode kind) {
    m_inputs.push_back(val);
    val->addUser(this);
    m_type = val->getType();
    m_op = kind;
  }

  UnaryOp(Instruction *val, Opcode kind, std::string &&name)
      : UnaryOp(val, kind) {
    setName(std::move(name));
  }

  Instruction *copy() const override {
    return new UnaryOp(m_inputs[0], m_op, getName());
  }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " " << std::endl;
  }

  bool is_vreg() const override { return true; }
};

class BinaryInstr : public Instruction {
public:
  BinaryInstr(Instruction *lhs, Instruction *rhs) {
    assert(lhs->getType() == rhs->getType());
    m_inputs.push_back(lhs);
    m_inputs.push_back(rhs);
    lhs->addUser(this);
    rhs->addUser(this);
  }

  BinaryInstr(Instruction *lhs, Instruction *rhs, std::string &&name) {
    setName(std::move(name));
  }

  bool is_vreg() const override { return true; }
};

class CmpInstr final : public BinaryInstr {
public:
  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " " << std::endl;
    m_inputs[1]->dumpRef(stream);
    stream << " " << std::endl;
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

  Instruction *copy() const override {
    return new CmpInstr(m_inputs[0], m_inputs[1], m_op, getName());
  }

  bool is_vreg() const override { return true; }
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
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    m_inputs[0]->dumpRef(stream);
    stream << " ";
    m_inputs[1]->dumpRef(stream);
    stream << " " << std::endl;
  }

  Instruction *copy() const override {
    return new BinaryOp(m_inputs[0], m_inputs[1], m_op, getName());
  }

  bool is_vreg() const override { return true; }
};

class CastInstr final : public Instruction {
public:
  CastInstr() { m_op = Opcode::CAST; }

  CastInstr(Instruction *val, Type type) : CastInstr() {
    m_cast = type;
    m_type = m_cast;
    m_inputs.push_back(val);
    val->addUser(this);
  }

  CastInstr(Instruction *val, Type type, std::string &&name)
      : CastInstr(val, type) {
    setName(std::move(name));
  }

  void dump(std::ostream &stream) override {
    // TODO
  }

  Instruction *copy() const override {
    // todo
    return nullptr;
  }

  bool is_vreg() const override { return true; }

private:
  Type m_cast;
};

class CallInstr : public Instruction {
public:
  CallInstr(Function *fn, Type type) : m_callee(fn) {
    m_op = Opcode::CALL;
    m_type = type;
  }

  CallInstr(Function *fn, Type type, std::string &&name) : CallInstr(fn, type) {
    setName(std::move(name));
  }

  auto argsBegin() { return m_inputs.begin(); }
  auto argsEnd() { return m_inputs.end(); }

  void addArg(Instruction *instr) {
    m_inputs.push_back(instr);
    instr->addUser(this);
  }

  Instruction *copy() const override {
    // todo
    return nullptr;
  }

  void dump(std::ostream &stream) override {
    dumpRef(stream);
    stream << ": " << OpcodeToStr(m_op) << " ";
    for (auto *input : m_inputs) {
      input->dumpRef(stream);
      stream << " ";
    }
    stream << std::endl;
  }

  Function *getCallee() const { return m_callee; }

private:
  Function *m_callee;
};

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
    Instruction *copy() const override {                                       \
      return new Constant<cty>(m_val, getName());                              \
    }                                                                          \
                                                                               \
    void dump(std::ostream &stream) override {                                 \
      dumpRef(stream);                                                         \
      stream << ": " << OpcodeToStr(m_op) << " ";                              \
      stream << Type(Type::jadety).getName() << " ";                           \
      stream << m_val << " ";                                                  \
      stream << std::endl;                                                     \
    }                                                                          \
                                                                               \
    cty getValue() const { return m_val; }                                     \
                                                                               \
    bool is_vreg() const override { return true; }                             \
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
CONSTANT(bool, I1);

template <typename T, typename... Args> T *BasicBlock::create(Args &&...args) {
  auto *elem = new T(args...);
  elem->setParent(this);
  elem->setId(m_iid);
  ++m_iid;

  m_instrs.insertBefore(m_inserter, elem);
  if constexpr (std::is_same_v<T, IfInstr>) {
    addSuccessor(elem->getFalseBB());
    addSuccessor(elem->getTrueBB());
  } else if constexpr (std::is_same_v<T, GotoInstr>) {
    addSuccessor(elem->getBB());
  } else if constexpr (std::is_same_v<T, PhiInstr>) {
    addPhi(elem);
  }

  return elem;
}

void replaceUsers(Instruction *oldInst, Instruction *newInst);
std::optional<std::int64_t> loadIntegerConst(Instruction *instr);
std::unique_ptr<Instruction> createIntegerConstant(std::int64_t val, Type type);

} // namespace jade
