#pragma once

#include "IR.hh"
#include "function.hh"
#include "graph.hh"
#include <set>
#include <stack>
#include <vector>

namespace jade {
class LinearOrder {
public:
    using GraphTy = BasicBlocksGraph;
    using Traits = GraphTraits<GraphTy>;

    std::vector<BasicBlock*> linearize(GraphTy& graph);
private:
    void processBasicBlock(BasicBlock* prev, BasicBlock* current);

    std::vector<BasicBlock*> m_linear{};
};
} // namespace jade
