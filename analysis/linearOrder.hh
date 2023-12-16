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

    LinearOrder(GraphTy& graph) : m_graph(graph) {
        auto lBuilder = LoopTreeBuilder<GraphTy>();
        m_loopTree = lBuilder.build(graph);
    }

    std::vector<BasicBlock*> linearize();
private:
    bool checkBlock(BasicBlock* bb);

    GraphTy& m_graph;
    LoopTree<GraphTy> m_loopTree;
    std::unordered_set<BasicBlock*> m_visited;
    std::vector<BasicBlock*> m_linear{};
};
} // namespace jade
