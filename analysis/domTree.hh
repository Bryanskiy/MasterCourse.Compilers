#pragma once

#include "graph.hh"
#include "dfs.hh"
#include <algorithm>
#include <cstddef>
#include <unordered_map>
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
    void computeSdoms();
    void computeIdoms();

private:
    std::vector<NodeTy> m_dfsLabels;

    class DSU {
    public:
        DSU(const GraphTy& G);

        NodeTy find(NodeTy v);
        NodeTy merge(NodeTy lhs, NodeTy rhs) {
            setParent(lhs, rhs);
        }

    private:
        NodeTy getParent(NodeTy node) {
            return m_dsu[m_indicies[node]];
        }

        std::size_t getNode(NodeTy node) {
            return m_indicies[node];
        }

        void setParent(NodeTy node, NodeTy parent) {
            m_dsu[m_indicies[node]] = parent;
        }

        NodeTy getLabel(NodeTy node) {
            return m_labels[m_indicies[node]];
        }

        void setLabel(NodeTy node, NodeTy parent) {
            m_labels[m_indicies[node]] = parent;
        }

    private:
        std::unordered_map<NodeTy, std::size_t> m_indicies;
        std::vector<NodeTy> m_dsu;
        // m_labels[i] stores the vertex v with minimum sdom,
        // lying on path from i to the dsu root.
        std::vector<NodeTy> m_labels;

        const std::vector<NodeTy>& m_semi;
    } m_dsu;
};

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::NodeTy
DominatorTreeBuilder<GraphTy>::DSU::find(NodeTy v) {
    auto parentNode = getParent(v);
    if (getParent(v) == v) {
        return v;
    }

    auto searchRes = find(parentNode);

    auto vIdx = getNode(v);
    auto parentIdx = getNode(parentNode);
    if(m_semi[parentIdx] < m_semi[vIdx]) {
        m_labels[vIdx] = m_labels[parentIdx];
    }

    setParent(v, searchRes);
    return m_labels[v];
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::build(GraphTy& G) {
    computeLabels(G);
    computeSdoms();
    computeIdoms();
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

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeSdoms() {
    for(auto it = m_dfsLabels.rbegin(); it != m_dfsLabels.rend()) {
        auto currenNode = *it;

        auto ancestorIt = Traits::inEdgeBegin(currenNode);
        auto ancestorEnd = Traits::inEdgeEnd(currenNode);

        for(; ancestorIt != ancestorEnd; ++it) {
            auto ancWithMinSdom = m_dsu.find(*ancestorIt);
            // auto calcSdom = std::min();
        }
    }
}

} // namespace jade
