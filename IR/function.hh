#pragma once

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

    static EdgesItTy outEdgeBegin(NodeTy node) { return node->successors().begin(); }
    static EdgesItTy outEdgeEnd(NodeTy node) { return node->successors().end(); }

    static EdgesItTy inEdgeBegin(NodeTy node) { return node->predecessors().begin(); }
    static EdgesItTy inEdgeEnd(NodeTy node) { return node->predecessors().end(); }
};

class Function {
public:

    BasicBlock* appendBB() {
        auto* bb = new BasicBlock{};
        m_bbs.push_back(bb);
        return bb;
    }

    Param* appendParam(Type type) {
        auto* param = new Param{type};
        m_params.push_back(param);
        return param;
    }

    auto getBasicBlocks() { return BasicBlocksGraph(m_bbs.borrow()); }

private:
    BasicBlocks m_bbs;
    Params m_params;
    std::size_t m_valCounter{0};
};

} // namespace jade
