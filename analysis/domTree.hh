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

template<typename GraphTy>
class DomTree {
public:
    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;

    bool dominate(NodeTy lhs, NodeTy rhs) {
        auto&& data = m_tree[Traits::id(lhs)].dominate;
        return std::find(data.begin(), data.end(), rhs) != data.end();
    }

    auto dominateBegin(NodeTy v) {
        return m_tree[Traits::id(v)].dominate.begin();
    }

    auto dominateEnd(NodeTy v) {
        return m_tree[Traits::id(v)].dominate.end();
    }

private:
    friend DominatorTreeBuilder<GraphTy>;

    void resize(std::size_t n) {
        m_tree.resize(n);
    }

    struct DomTreeNode {
        NodeTy idom;
        std::vector<NodeTy> dominate;
    };

    std::vector<DomTreeNode> m_tree;
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
    using DomTreeTy = DomTree<GraphTy>;

    DomTreeTy build(GraphTy& G);

private:
    void reset();
    void initState(GraphTy& G);
    void computeLabels(NodeTy node);
    void computeSdoms();
    void computeIdoms();

private:
    std::vector<NodeTy> m_dfsLabels;
    std::set<std::size_t> m_visited;
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
        DSU() = default;
        DSU(GraphTy& G) {
            m_dsu.resize(Traits::nodesCount(G));
            m_labels.resize(Traits::nodesCount(G));

            auto it = Traits::nodesBegin(G);
            for(; it != Traits::nodesEnd(G); ++it) {
                m_dsu[Traits::id(&*it)] = &*it;
                m_labels[Traits::id(&*it)] = &*it;
            }
        }

        NodeTy find(const std::vector<std::size_t>& semi, NodeTy v);
        void merge(NodeTy parent, NodeTy node) {
            setParent(parent, node);
        }

        NodeTy getParent(NodeTy node) {
            return m_dsu[Traits::id(node)];
        }

        void setParent(NodeTy parent, NodeTy node) {
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
    } m_dsu;
};

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::NodeTy
DominatorTreeBuilder<GraphTy>::DSU::find(const std::vector<std::size_t>& semi, NodeTy v) {
    auto parentNode = getParent(v);
    if (parentNode == v) {
        return v;
    }

    auto searchRes = find(semi, parentNode);

    if(semi[Traits::id(parentNode)] < semi[Traits::id(v)]) {
        m_labels[Traits::id(v)] = m_labels[Traits::id(parentNode)];
    }

    setParent(searchRes, v);
    return m_labels[Traits::id(v)];
}

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::DomTreeTy
DominatorTreeBuilder<GraphTy>::build(GraphTy& G) {
    reset();
    initState(G);
    computeLabels(Traits::entry(G));
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
    m_dfsLabels.reserve(Traits::nodesCount(G));
    m_dfsParents.resize(Traits::nodesCount(G));
    m_sdoms.resize(Traits::nodesCount(G));
    m_idoms.resize(Traits::nodesCount(G));
    m_bucket.resize(Traits::nodesCount(G));
    m_domTree.resize(Traits::nodesCount(G));

    m_dsu = DSU(G);
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeLabels(NodeTy node) {
    auto counter = m_dfsLabels.size();
    if (counter == 0) {
        m_dfsParents[Traits::id(node)] = node;
    }

    m_dfsLabels.push_back(node);
    m_visited.insert(Traits::id(node));
    m_sdoms[Traits::id(node)] = counter;
    m_idoms[Traits::id(node)] = counter;

    auto it = Traits::outEdgeBegin(node);
    for(; it != Traits::outEdgeEnd(node); ++it) {
        if (!m_visited.count(Traits::id(*it))) {
            m_dfsParents[Traits::id(*it)] = node;
            computeLabels(*it);
        }
    }
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeSdoms() {
    for(auto it = m_dfsLabels.rbegin(); it != m_dfsLabels.rend(); ++it) {
        auto currenNode = *it;

        auto ancestorIt = Traits::inEdgeBegin(currenNode);
        auto ancestorEnd = Traits::inEdgeEnd(currenNode);

        for(; ancestorIt != ancestorEnd; ++ancestorIt) {
            auto ancWithMinSdom = m_dsu.find(m_sdoms, *ancestorIt);
            m_sdoms[Traits::id(currenNode)] = std::min(m_sdoms[Traits::id(currenNode)], m_sdoms[Traits::id(ancWithMinSdom)]);

            if (currenNode != *m_dfsLabels.begin()) {
                m_dsu.merge(m_dfsParents[Traits::id(currenNode)], currenNode);
                m_bucket[m_sdoms[Traits::id(currenNode)]].push_back(currenNode);
            }
        }

        for(auto&& dominatee : m_bucket[Traits::id(currenNode)]) {
            auto minSdom = m_dsu.find(m_sdoms, dominatee);
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
        m_domTree.m_tree[Traits::id(node)].idom = immNode;
        m_domTree.m_tree[Traits::id(immNode)].dominate.push_back(node);
    }
}

} // namespace jade
