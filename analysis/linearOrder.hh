#pragma once

#include <set>
#include <stack>
#include <unordered_set>
#include <vector>

#include "IR.hh"
#include "function.hh"
#include "graph.hh"
#include "loopAnalyser.hh"

namespace jade {
class LinearOrder {
public:
    using GraphTy = BasicBlocksGraph;
    using Traits = GraphTraits<GraphTy>;

    LinearOrder(Function& function, bool skip_cond = true) : m_graph(function), m_skip_cond(skip_cond) {
        auto lBuilder = LoopTreeBuilder<GraphTy>();
        auto graph  = function.getBasicBlocks();
        m_loopTree = lBuilder.build(graph);
    }

    std::vector<BasicBlock*> linearize();
private:
    bool checkBlock(BasicBlock* bb);
    void processCondition(BasicBlock* bb);
    BasicBlock* insertGotoBB(BasicBlock* source, BasicBlock* dst);
    bool shouldInverseBranches(BasicBlock* falseBranch, BasicBlock* trueBranch, BasicBlock* cond);

    Function& m_graph;
    LoopTree<GraphTy> m_loopTree;
    std::set<BasicBlock*> m_visited;
    std::vector<BasicBlock*> m_linear{};
    bool m_skip_cond = true;
};
} // namespace jade
