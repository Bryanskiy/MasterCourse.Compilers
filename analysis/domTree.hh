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

// algo description:
// https://tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
template<typename GraphTy>
class DominatorTreeBuilder {
public:
    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;
    using EdgesItTy = typename Traits::EdgesItTy;

    void build(const GraphTy& G);

private:
    void reset();
    void initState(const GraphTy& G);
    void computeLabels(const GraphTy& G);
    void computeSdoms();
    void computeIdoms();

private:

private:
    //  mapping of i’th node to its new index, equal to the arrival time of node in dfs tree.
    std::vector<NodeTy> m_dfsLabels;
    // parent of node i in dfs tree
    std::vector<NodeTy> m_dfsParents;
    // label of semi-dominator of the i’th node
    std::vector<NodeTy> m_sdoms;
    // label of immediate-dominator of the i’th node. Initially
    std::vector<NodeTy> m_idoms;
    // For a vertex i, it stores a list of vertices for which i is the semi-dominator.
    std::vector<std::vector<NodeTy>> m_bucket;

    class DSU {
    public:
        DSU(const GraphTy& G);

        NodeTy find(NodeTy v);
        NodeTy merge(NodeTy lhs, NodeTy rhs) {
            setParent(lhs, rhs);
        }

    private:
        NodeTy getParent(NodeTy node) {
            return m_dsu[Traits::id(node)];
        }

        void setParent(NodeTy node, NodeTy parent) {
            m_dsu[Traits::id(node)] = parent;
        }

        NodeTy getLabel(NodeTy node) {
            return m_labels[Traits::id(node)];
        }

        void setLabel(NodeTy node, NodeTy parent) {
            m_labels[Traits::id(node)] = parent;
        }

    private:
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

    if(m_semi[Traits::id(parentNode)] < m_semi[v]) {
        m_labels[v] = m_labels[Traits::id(parentNode)];
    }

    setParent(v, searchRes);
    return m_labels[v];
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::build(const GraphTy& G) {
    reset();
    initState(G);
    computeLabels(G);
    computeSdoms();
    computeIdoms();
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::reset() {
    m_dfsLabels.clear();
    m_dfsParents.clear();
    m_sdoms.clear();
    m_bucket.clear();
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::initState(const GraphTy& G) {
    m_dfsLabels.resize(Traits::nodesCount(G));
    m_dfsParents.resize(Traits::nodesCount(G));
    m_sdoms.resize(Traits::nodesCount(G));
    m_bucket.resize(Traits::nodesCount(G));
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeLabels(const GraphTy& G) {
    auto dfsIt = DFSIterator<GraphTy>::begin(G);
    auto end = DFSIterator<GraphTy>::end(G);

    while (dfsIt != end) {
        m_dfsLabels.push_back(*dfsIt);

        m_sdoms[Traits::id(*dfsIt)] = Traits::id(*dfsIt);
        m_idoms[Traits::id(*dfsIt)] = Traits::id(*dfsIt);

        auto prev = *dfsIt;
        ++dfsIt;
        m_dfsParents[Traits::id(prev)] = *dfsIt;
    }
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeSdoms() {
    for(auto it = m_dfsLabels.rbegin(); it != m_dfsLabels.rend(); ++it) {
        auto currenNode = *it;

        auto ancestorIt = Traits::inEdgeBegin(currenNode);
        auto ancestorEnd = Traits::inEdgeEnd(currenNode);

        for(; ancestorIt != ancestorEnd; ++it) {
            auto ancWithMinSdom = m_dsu.find(*ancestorIt);
            m_sdoms[Traits::id(*it)] = std::min(m_sdoms[Traits::id(*it)], m_sdoms[Traits::id(*ancWithMinSdom)]);

            if (it != m_dfsLabels.rbegin()) {
                m_dsu.merge(m_dfsParents[*it], *it);
                m_bucket[m_sdoms[Traits::id(*it)]].push_back(*it);
            }
        }

        for(auto&& dominatee : m_bucket[m_sdoms[Traits::id(*it)]]) {
            auto minSdom = m_dsu.find(dominatee);
            if (m_sdoms[minSdom] == m_sdoms[dominatee]) {
                m_idoms[Traits::id(*dominatee)] = m_sdoms[Traits::id(*dominatee)];
            } else {
                m_idoms[Traits::id(*dominatee)] = minSdom;
            }
        }
    }
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeIdoms() {
    for(auto&& node : m_dfsLabels) {
        if(m_idoms[Traits::id(node)] != m_idoms[m_sdoms[Traits::id(node)]]) {
            m_idoms[Traits::id(node)] = m_idoms[m_idoms[Traits::id(node)]];
        }
    }
}

} // namespace jade
