#pragma once

#include <memory>
#include <vector>

#include "IR.hh"
#include "ilist.hh"
#include "graph.hh"

namespace jade {

using BasicBlocks = IList<BasicBlock>;
using BorrowedBBs = IList<BasicBlock, IListBorrower<BasicBlock>>;

class Param : public Value, public IListNode {
public:
    Param(Type type) : Value{type} {}
};
using Params =  IList<Param>;

class BasicBlocksGraph {
private:
    BorrowedBBs m_bbs;

public:
    // Graph Trait impl
    using NodeTy = BasicBlock*;
    using EdgesItTy = decltype(m_bbs.begin().getPtr()->successors().begin());

    static NodeTy entry(BasicBlocksGraph& G) {
        return G.m_bbs.begin().getPtr();
    }

    static EdgesItTy edgeBegin(NodeTy node) { return node->successors().begin(); }
    static EdgesItTy edgeEnd(NodeTy node) { return node->successors().end(); }
public:
    BasicBlocksGraph(BorrowedBBs c) : m_bbs{c} {}

    auto nodes() { return Range{m_bbs.begin(), m_bbs.end()}; }
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
};

} // namespace jade
