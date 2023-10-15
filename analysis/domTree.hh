#pragma once

#include "graph.hh"
#include "dfs.hh"
#include <vector>

namespace jade {

template<typename NodeTy>
class DomTreeNode {
    NodeTy m_dominator;
    NodeTy m_dominated;
};

template<typename GraphTy>
class DominatorTreeBuilder {
public:
    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;
    using EdgesItTy = typename Traits::EdgesItTy;

    void build(GraphTy& G);

private:
    void computeLabels(GraphTy& G);

private:
    std::vector<NodeTy> m_dfsLabels;
};

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::build(GraphTy& G) {
    computeLabels(G);
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeLabels(GraphTy& G) {
    auto dfsIt = DFSIterator<GraphTy>::begin(G);
    auto end = DFSIterator<GraphTy>::end(G);

    while (dfsIt != end) {
        m_dfsLabels.push_back(*dfsIt);
        ++dfsIt;
    }
}

} // namespace jade
