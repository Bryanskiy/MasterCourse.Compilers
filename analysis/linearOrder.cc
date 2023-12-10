#include "linearOrder.hh"
#include "IR.hh"
#include "graph.hh"
#include <cassert>

namespace jade {

std::vector<BasicBlock*> LinearOrder::linearize(GraphTy& graph) {
    auto it = RPOIterator<GraphTy>::begin(graph);
    auto end = RPOIterator<GraphTy>::end(graph);

    if (it == end) {
        return {};
    }

    BasicBlock* prev = nullptr;
    for(; it != end; ++it) {
        BasicBlock* currentBB = *it;
        if (prev) {
            processBasicBlock(prev, currentBB);
        }
        m_linear.push_back(currentBB);
        prev = currentBB;
    }
    return m_linear;
}

void LinearOrder::processBasicBlock(BasicBlock* prev, BasicBlock* current) {
    auto successors = prev->collectSuccessors();
    switch (successors.size()) {
        case 1: {
            break;
        }
        case 2: {
            auto trueBranch = successors[0];
            auto falseBranch = successors[1];
            if(current == trueBranch) {
                current->inverseCondition();
            } else if()

            break;
        }
        default: assert(0);
    }
}

}
