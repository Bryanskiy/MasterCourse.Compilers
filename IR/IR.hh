#pragma once

#include <array>
#include <cassert>
#include <memory>
#include <ostream>
#include <vector>
#include <cstdint>
#include <set>
#include <list>

#include "opcodes.hh"
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

    Type() : m_tag{Tag::None} {}
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
    std::string m_name;
};

class InstrBulder;
class Instruction;

template<typename IT>
class Range {
    IT m_begin;
    IT m_end;

public:
    IT begin() { return m_begin; }
    IT end() { return m_end; }

    Range(IT begin, IT end) : m_begin(begin), m_end(end) {}
};

template<typename IT>
Range(IT begin, IT end) -> Range<IT>;

class BasicBlock final : public IListNode {
public:

    auto successors() { return Range(m_succs.begin(), m_succs.end()); }
    auto predecessors() { return Range(m_preds.begin(), m_preds.end()); }

    void addSuccessor(BasicBlock* succs) { m_succs.push_back(succs); }
    void addPredecessor(BasicBlock* pred) { m_preds.push_back(pred); }

    void removeSuccessor(BasicBlock* succs) { m_succs.remove(succs); }
    void removePredecessor(BasicBlock* pred) { m_preds.remove(pred); }

private:
    friend InstrBulder;

    IList<Instruction> m_instrs;
    std::list<BasicBlock*> m_preds;
    std::list<BasicBlock*> m_succs;
};

class InstrBulder final {
public:
    using iterator = IListIterator<Instruction>;

    InstrBulder(BasicBlock* bb) : m_bb{bb} {
        m_inserter = m_bb->m_instrs.end();
    }

    void setInsertPoint(Instruction* inserter) {
        m_inserter = iterator(inserter);
    }

    template<typename T, typename ...Args>
    T* create(Args&&... args);

private:
    BasicBlock* m_bb{nullptr};
    iterator m_inserter{nullptr};
};

class Instruction : public Value, public IListNode {
public:
    Instruction() = default;
    Instruction(Type type) : Value{type} {}
    Instruction(Type type, BasicBlock* bb) : Value{type}, m_bb(bb) {}
    virtual ~Instruction() = default;

    BasicBlock* getParent() { return m_bb; }
    void setParent(BasicBlock* bb) { m_bb = bb; }
    virtual void dump(std::ostream& stream) = 0;

    friend InstrBulder;

protected:
    Opcode m_op;
private:
    BasicBlock* m_bb{nullptr};
};

class IfInstr final : public Instruction {
public:
    IfInstr() {
        m_op = Opcode::IF;
    }

    IfInstr(Value* cond, BasicBlock* false_, BasicBlock* true_) : IfInstr() {
        m_cond = cond; m_false_bb = false_; m_true_bb = true_;
    }

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
    GotoInstr() {
        m_op = Opcode::GOTO;
    }

    GotoInstr(BasicBlock* bb) : GotoInstr() {
        m_bb = bb;
    }

    void dump(std::ostream& stream) override {
        // TODO
    }
private:
    friend InstrBulder;
    BasicBlock* m_bb{nullptr};
};

class RetInstr final : public Instruction {
public:
    RetInstr() {
        m_op = Opcode::RET;
    }

    RetInstr(Value* v) : RetInstr() {
        m_v = v;
    }

    void dump(std::ostream& stream) override {
        // TODO
    }
private:
    Value* m_v;
};

class PhiInstr final : public Instruction {
public:
    PhiInstr(Type type) : Instruction{type} {
        m_op = Opcode::PHI;
    }

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
    void dump(std::ostream& stream) override {
        // TODO
    }

    CmpInstr(Value* lhs, Value* rhs, Opcode kind) : BinaryInstr(lhs, rhs), m_kind(kind) {
        m_type = Type::create<Type::I1>();
    }

private:
    Opcode m_kind;
};

class BinaryOp final : public BinaryInstr {
public:

    BinaryOp(Value* lhs, Value* rhs, Opcode kind) : BinaryInstr(lhs, rhs), m_kind(kind) {
        m_type = lhs->getType();
    }

    void dump(std::ostream& stream) override {
        // TODO
    }
private:
    Opcode m_kind;
};

class CastInstr final : public Instruction {
public:
    CastInstr() {
        m_op = Opcode::CAST;
    }

    CastInstr(Value* val, Type type) : CastInstr() {
        m_cast = type; m_type = m_cast; m_val = val;
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
#define CONSTANT(cty, jadety)                                                    \
template <>                                                                      \
class Constant<cty> : public Instruction {                                       \
public:                                                                          \
    Constant(cty val) : Instruction{Type::jadety} {                              \
        m_op = Opcode::CONST;                                                    \
        m_val = val;                                                             \
    }                                                                            \
                                                                                 \
    void dump(std::ostream& stream) override {                                   \
                                                                                 \
    }                                                                            \
                                                                                 \
    cty getValue() const { return m_val; }                                       \
private:                                                                         \
    cty m_val;                                                                   \
};                                                                               \
                                                                                 \
using Const##jadety = Constant<cty>;

CONSTANT(std::int64_t, I64);
CONSTANT(std::int32_t, I32);
CONSTANT(std::int16_t, I16);
CONSTANT(std::int8_t, I8);

template<typename T, typename ...Args>
T* InstrBulder::create(Args&&... args) {
    auto* elem = new T(args...);
    elem->setParent(m_bb);
    m_bb->m_instrs.insertBefore(m_inserter, elem);
    return elem;
}

} // namespace jade
