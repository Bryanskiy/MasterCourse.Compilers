#pragma once

#include "graph.hh"
#include "dfs.hh"
#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace jade {

template<typename GraphTy>
class DominatorTreeBuilder;

template<typename NodeTy>
class DomTree {
public:
    bool dominate(NodeTy lhs, NodeTy rhs) {
        auto dominated = m_nodes.find(lhs);
        if (dominated != m_nodes.end()) {
            return std::find(dominated->begin(), dominated->end(), rhs) != dominated->end();
        }

        return false;
    }
    // vertices dominatedBy(v)
    // vertices dominate(v)
private:
    template<typename Ty>
    friend class DominatorTreeBuilder;

    std::unordered_map<NodeTy, std::vector<NodeTy>> m_nodes;
};

template<typename NodeTy>
struct GraphTraits<DomTree<NodeTy>> {
    // TODO
};

// algo description:
// https://tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
template<typename GraphTy>
class DominatorTreeBuilder {
public:
    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;
    using EdgesItTy = typename Traits::EdgesItTy;
    using DomTreeTy = DomTree<NodeTy>;

    DomTreeTy build(GraphTy& G);

private:
    void reset();
    void initState(GraphTy& G);
    void computeLabels(GraphTy& G);
    void computeSdoms();
    void computeIdoms();

private:
    std::vector<NodeTy> m_dfsLabels;
    // parent of node i in dfs tree
    std::vector<NodeTy> m_dfsParents;
    // label of semi-dominator of the i’th node
    std::vector<std::size_t> m_sdoms;
    // label of immediate-dominator of the i’th node. Initially
    std::vector<std::size_t> m_idoms;
    // For a vertex i, it stores a list of vertices for which i is the semi-dominator.
    std::vector<std::vector<NodeTy>> m_bucket;

    DomTreeTy m_domTree;
    class DSU {
    public:
        DSU(const std::vector<size_t>* semi) : m_semi(semi) {
            if (m_semi) {
                m_dsu.resize(m_semi->size());
                m_labels.resize(m_semi->size());
            }
        }

        NodeTy find(NodeTy v);
        void merge(NodeTy lhs, NodeTy rhs) {
            setParent(lhs, rhs);
        }

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

        const std::vector<std::size_t>* m_semi{nullptr};
    } m_dsu{nullptr};
};

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::NodeTy
DominatorTreeBuilder<GraphTy>::DSU::find(NodeTy v) {
    auto parentNode = getParent(v);
    if (getParent(v) == v) {
        return v;
    }

    auto searchRes = find(parentNode);

    if(m_semi[Traits::id(parentNode)] < m_semi[Traits::id(v)]) {
        m_labels[Traits::id(v)] = m_labels[Traits::id(parentNode)];
    }

    setParent(v, searchRes);
    return m_labels[Traits::id(v)];
}

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::DomTreeTy
DominatorTreeBuilder<GraphTy>::build(GraphTy& G) {
    reset();
    initState(G);
    computeLabels(G);
    computeSdoms();
    computeIdoms();

    return m_domTree;
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::reset() {
    m_dfsLabels.clear();
    m_dfsParents.clear();
    m_sdoms.clear();
    m_bucket.clear();
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::initState(GraphTy& G) {
    m_dfsLabels.resize(Traits::nodesCount(G));
    m_dfsParents.resize(Traits::nodesCount(G));
    m_sdoms.resize(Traits::nodesCount(G));
    m_bucket.resize(Traits::nodesCount(G));

    m_dsu = DSU(&m_sdoms);
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeLabels(GraphTy& G) {
    auto dfsIt = DFSIterator<GraphTy>::begin(G);
    auto end = DFSIterator<GraphTy>::end(G);

    while (dfsIt != end) {
        m_dfsLabels.push_back(*dfsIt);

        m_sdoms[Traits::id(*dfsIt)] = Traits::id(*dfsIt);
        m_idoms[Traits::id(*dfsIt)] = Traits::id(*dfsIt);
        m_dsu.setLabel(*dfsIt, *dfsIt);
        m_dsu.setParent(*dfsIt, *dfsIt);

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
            m_sdoms[Traits::id(*it)] = std::min(m_sdoms[Traits::id(*it)], m_sdoms[Traits::id(ancWithMinSdom)]);

            if (it != m_dfsLabels.rbegin()) {
                m_dsu.merge(m_dfsParents[Traits::id(*it)], *it);
                m_bucket[m_sdoms[Traits::id(*it)]].push_back(*it);
            }
        }

        for(auto&& dominatee : m_bucket[m_sdoms[Traits::id(*it)]]) {
            auto minSdom = m_dsu.find(dominatee);
            if (m_sdoms[Traits::id(minSdom)] == m_sdoms[Traits::id(dominatee)]) {
                m_idoms[Traits::id(dominatee)] = m_sdoms[Traits::id(dominatee)];
            } else {
                m_idoms[Traits::id(dominatee)] = Traits::id(minSdom);
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

        auto immNode = m_dfsLabels[m_idoms[Traits::id(node)]];
        m_domTree.m_nodes[immNode].push_back(node);
    }
}

} // namespace jade
