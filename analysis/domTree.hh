#pragma once

#include "graph.hh"
#include "dfs.hh"
#include <algorithm>
#include <cstddef>
#include <ostream>
#include <set>
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
        if (Traits::id(lhs) == Traits::id(rhs)) {
            return true;
        }

        auto&& data = m_tree[lhs].dominate;
        for (auto && node: data) {
            if (Traits::id(node) == Traits::id(lhs)) {
                continue;
            }

            if (Traits::id(node) == Traits::id(rhs)) {
                return true;
            }

            if (dominate(node, rhs)) {
                return true;
            }
        }
        return false;
    }

    void dump(std::ostream& stream) {
        for(auto&& [node, dnode]: m_tree) {
            stream << "node: " << Traits::id(node) << std::endl;
            stream << "    idom:" << Traits::id(dnode.idom) << std::endl;
            stream << "    dominates:"  << std::endl;
            for (auto&& dominatee: dnode.dominate) {
                stream << "        "  << Traits::id(dominatee) << std::endl;
            }
        }
    }

    auto dominateBegin(NodeTy v) {
    }

    auto dominateEnd(NodeTy v) {
    }

private:
    friend DominatorTreeBuilder<GraphTy>;

    struct DomTreeNode {
        NodeTy idom;
        std::vector<NodeTy> dominate;
    };

    std::unordered_map<NodeTy, DomTreeNode> m_tree;
};

template<typename NodeTy>
struct GraphTraits<DomTree<NodeTy>> {
    // TODO
};

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

    class DSU;
    void computeSdoms(DSU& dsu);
    void computeIdoms();

private:
    using IndexMap = std::unordered_map<std::size_t, std::size_t>;

    IndexMap m_dfsLabels;
    std::vector<NodeTy> m_dfsNodes;
    std::set<std::size_t> m_visited;
    // parent of node i in dfs tree
    IndexMap m_dfsParents;
    // label of semi-dominator of the i’th node
    std::vector<std::size_t> m_sdoms;
    // label of immediate-dominator of the i’th node
    std::vector<std::size_t> m_idoms;
    // For a vertex i, it stores a list of vertices for which i is the semi-dominator
    std::vector<std::vector<std::size_t>> m_bucket;

    DomTreeTy m_domTree;
    class DSU {
    public:
        DSU(const IndexMap& dfsLabels, const std::vector<std::size_t>& sdoms, const std::vector<NodeTy>& dfsNodes) :
            m_dfsLabels{dfsLabels}, m_sdoms{sdoms} {
                m_dsu = dfsNodes;
                m_labels = dfsNodes;
            }

        NodeTy find(NodeTy v);
        void merge(NodeTy parent, NodeTy node) {
            setParent(parent, node);
        }

        NodeTy getParent(NodeTy node) {
            return m_dsu[m_dfsLabels.at(Traits::id(node))];
        }

        void setParent(NodeTy parent, NodeTy node) {
            m_dsu[m_dfsLabels.at(Traits::id(node))] = parent;
        }

        NodeTy getLabel(NodeTy node) {
            return m_labels[m_dfsLabels.at(Traits::id(node))];
        }

        void setLabel(NodeTy node, NodeTy parent) {
            m_labels[m_dfsLabels.at(Traits::id(node))] = parent;
        }

        std::size_t getSemi(NodeTy node) {
            return m_sdoms[m_dfsLabels.at(Traits::id(node))];
        }

    private:
        void compress(NodeTy node);
    private:
        const IndexMap& m_dfsLabels;
        const std::vector<std::size_t>& m_sdoms;

        std::vector<NodeTy> m_dsu;
        // m_labels[i] stores the vertex v with minimum sdom,
        // lying on path from i to the dsu root.
        std::vector<NodeTy> m_labels;
    };
};

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::NodeTy
DominatorTreeBuilder<GraphTy>::DSU::find(NodeTy v) {
    compress(v);
    return getLabel(v);
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::DSU::compress(NodeTy node) {
    auto parentNode = getParent(node);
    if (parentNode == node) {
        return;
    }

    compress(parentNode);

    setParent(getParent(parentNode), node);

    if(getSemi(parentNode) < getSemi(node)) {
        setLabel(node, getParent(node));
    }
}

template<typename GraphTy>
typename DominatorTreeBuilder<GraphTy>::DomTreeTy
DominatorTreeBuilder<GraphTy>::build(GraphTy& G) {
    reset();
    initState(G);
    computeLabels(Traits::entry(G));
    auto dsu = DSU(m_dfsLabels, m_sdoms, m_dfsNodes);
    computeSdoms(dsu);
    computeIdoms();

    return m_domTree;
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::reset() {
    m_dfsNodes.clear();
    m_dfsLabels.clear();
    m_dfsParents.clear();
    m_sdoms.clear();
    m_bucket.clear();
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::initState(GraphTy& G) {
    m_dfsNodes.reserve(Traits::nodesCount(G));
    m_sdoms.resize(Traits::nodesCount(G));
    m_idoms.resize(Traits::nodesCount(G));
    m_bucket.resize(Traits::nodesCount(G));

    m_dfsParents[Traits::id(Traits::entry(G))] = 0;
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeLabels(NodeTy node) {
    auto counter = m_visited.size();

    m_dfsNodes.push_back(node);
    m_visited.insert(Traits::id(node));

    m_dfsLabels[Traits::id(node)] = counter;
    m_sdoms[counter] = counter;
    m_idoms[counter] = counter;

    auto it = Traits::outEdgeBegin(node);
    for(; it != Traits::outEdgeEnd(node); ++it) {
        if (!m_visited.count(Traits::id(*it))) {
            m_dfsParents[Traits::id(*it)] = counter;
            computeLabels(*it);
        }
    }
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeSdoms(DSU& dsu) {
    std::size_t currentIdx = m_dfsNodes.size() - 1;
    for(auto it = m_dfsNodes.rbegin(); it != m_dfsNodes.rend(); ++it, --currentIdx) {
        auto currenNode = *it;

        auto ancestorIt = Traits::inEdgeBegin(currenNode);
        auto ancestorEnd = Traits::inEdgeEnd(currenNode);

        for(; ancestorIt != ancestorEnd; ++ancestorIt) {
            auto ancWithMinSdom = dsu.find(*ancestorIt);
            auto ancWithMinSdomIdx = m_dfsLabels[Traits::id(ancWithMinSdom)];

            m_sdoms[currentIdx] = std::min(m_sdoms[currentIdx], m_sdoms[ancWithMinSdomIdx]);
            if (*it != *m_dfsNodes.begin()) {
                m_bucket[m_sdoms[currentIdx]].push_back(currentIdx);
            }
        }

        for(auto&& dominateeIdx : m_bucket[currentIdx]) {
            auto minSdom = dsu.find(m_dfsNodes[dominateeIdx]);
            auto minSdomIdx = m_dfsLabels[Traits::id(minSdom)];

            if (m_sdoms[minSdomIdx] == m_sdoms[dominateeIdx]) {
                m_idoms[dominateeIdx] = m_sdoms[dominateeIdx];
            } else {
                m_idoms[dominateeIdx] = minSdomIdx;
            }
        }

        if (*it != *m_dfsNodes.begin()) {
            dsu.merge(m_dfsNodes[m_dfsParents[Traits::id(currenNode)]], currenNode);
        }
    }
}

template<typename GraphTy>
void DominatorTreeBuilder<GraphTy>::computeIdoms() {
    std::size_t id = 0;
    for(auto&& node : m_dfsNodes) {
        if(m_idoms[id] != m_sdoms[id]) {
            m_idoms[id] = m_idoms[m_idoms[id]];
        }

        auto immNode = m_dfsNodes[m_idoms[id]];
        m_domTree.m_tree[node].idom = immNode;
        m_domTree.m_tree[immNode].dominate.push_back(node);

        ++id;
    }
}

} // namespace jade
