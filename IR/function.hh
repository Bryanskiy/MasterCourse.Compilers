#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "IR.hh"
#include "ilist.hh"
#include "graph.hh"

namespace jade {

using BasicBlocks = IList<BasicBlock>;
using BasicBlocksRef = IList<BasicBlock, IListBorrower<BasicBlock>>;

class Param : public Value, public IListNode {
public:
    Param(Type type) : Value{type} {}
};
using Params =  IList<Param>;

class BasicBlocksGraph {
private:
    BasicBlocksRef m_bbs;

public:
    BasicBlocksGraph() = default;
    BasicBlocksGraph(BasicBlocksRef c) : m_bbs{c} {}

    auto nodes() { return Range{m_bbs.begin(), m_bbs.end()}; }
};

template<>
struct GraphTraits<BasicBlocksGraph> {
    using NodeTy = BasicBlock*;
    using EdgesItTy = decltype(BasicBlocksGraph().nodes().begin().getPtr()->successors().begin());

    static NodeTy entry(BasicBlocksGraph& G) {
        return G.nodes().begin().getPtr();
    }

    static std::size_t id(NodeTy node) { return node->getId(); }

    static EdgesItTy outEdgeBegin(NodeTy node) { return node->successors().begin(); }
    static EdgesItTy outEdgeEnd(NodeTy node) { return node->successors().end(); }

    static EdgesItTy inEdgeBegin(NodeTy node) { return node->predecessors().begin(); }
    static EdgesItTy inEdgeEnd(NodeTy node) { return node->predecessors().end(); }
};

class Function {
public:

    template<typename T, typename ...Args>
    T* create(Args&&... args);

    auto getBasicBlocks() { return BasicBlocksGraph(m_bbs.borrow()); }

private:
    BasicBlocks m_bbs;
    Params m_params;
    std::size_t m_valCounter{0};
};

template<typename T, typename ...Args>
T* Function::create(Args&&... args) { assert(0); }

template<>
inline BasicBlock* Function::create<BasicBlock>() {
    auto* bb = new BasicBlock{};
    std::size_t id = 0;
    if (!m_bbs.empty()) {
        id = m_bbs.getLast()->getId() + 1;
    }
    bb->setId(id);
    m_bbs.push_back(bb);
    return bb;
}

template<>
inline Param* Function::create<Param, Type>(Type&& type) {
    auto* param = new Param{type};
    m_params.push_back(param);
    return param;
}

} // namespace jade
