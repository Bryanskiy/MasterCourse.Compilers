#pragma once

#include "graph.hh"
#include "domTree.hh"
#include <ostream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace jade {

template<typename GraphTy>
class LoopTreeBuilder;

template<typename GraphTy>
class LoopTreeNode final {
public:
    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;
public:
    LoopTreeNode(NodeTy header, bool isReducible) :
        m_header{header}, m_isReducible{isReducible} {}

    LoopTreeNode(NodeTy header) : m_header{header} {}

    LoopTreeNode* getOuter() const { return m_outer; }
    NodeTy getHeader() const { return m_header; }
    bool isReducible() const { return m_isReducible; }

    void dump(std::ostream& out) {
        dumpInner(out, "");
    }
private:

    void insertInnerLoop(LoopTreeNode* node) {
        m_inners.push_back(node);
    }

    void setOuter(LoopTreeNode* node) {
        m_outer = node;
    }

    void addBackEdge(NodeTy node) {
        m_nodes.push_back(node);
        m_backEdges.push_back(node);
    }

    void insertNode(NodeTy node) {
        m_nodes.push_back(node);
    }

    auto backEdgesBegin() {
        return m_backEdges.begin();
    }

    auto backEdgesEnd() {
        return m_backEdges.end();
    }

    void addReducibility(bool isReducible) {
        m_isReducible = isReducible;
    }

    void dumpInner(std::ostream& out, std::string indent) {
        out << indent << "header: " << Traits::id(m_header) << std::endl;
        out << indent << "back edges: ";
        for (auto backEdge: m_backEdges) {
            out << indent << Traits::id(backEdge) << " ";
        }
        out << std::endl;
        out << indent << "nodes: ";
        for (auto node: m_nodes) {
            out << indent << Traits::id(node) << " ";
        }
        out << std::endl;
        for (auto inner: m_inners) {
            dumpInner(out, indent + "  ");
        }
    }
private:
    friend LoopTreeBuilder<GraphTy>;

    NodeTy m_header;
    std::vector<NodeTy> m_backEdges;
    std::vector<NodeTy> m_nodes;
    LoopTreeNode* m_outer{nullptr};
    std::vector<LoopTreeNode*> m_inners;

    bool m_isReducible{false};
};

template<typename GraphTy>
class LoopTree {
public:
    using LoopTreeNodeTy = LoopTreeNode<GraphTy>;

private:
    friend LoopTreeBuilder<GraphTy>;

    std::vector<LoopTreeNodeTy> m_arena;
    LoopTreeNodeTy* m_head{nullptr};
};

template<typename GraphTy>
class LoopTreeBuilder final {
public:

    using Traits = GraphTraits<GraphTy>;
    using NodeTy = typename Traits::NodeTy;
    using EdgesItTy = typename Traits::EdgesItTy;
    using LoopTreeNodeTy = LoopTreeNode<GraphTy>;
    using LoopTreeTy = LoopTree<GraphTy>;
    using DomTreeTy = DomTree<GraphTy>;

    LoopTreeTy build(GraphTy& G);

private:
    void init(GraphTy& G);
    void collectBackEdges(NodeTy node);
    void appendLoopInfo(NodeTy header, NodeTy backEdgeSrc);
    void populate(GraphTy& G);
    void populateInner(LoopTreeNodeTy* loop, NodeTy node);
    LoopTreeTy finalize();

    using ColorMap = std::unordered_map<NodeTy, Gcolor>;
private:
    DomTreeTy m_domTree;
    ColorMap m_colors;
    std::unordered_map<NodeTy, LoopTreeNodeTy*> m_loopsMap;
    std::vector<LoopTreeNodeTy> m_arena;
    std::vector<NodeTy> m_dfsNodes;
};

template<typename GraphTy>
void LoopTreeBuilder<GraphTy>::init(GraphTy& G) {
    m_dfsNodes.reserve(Traits::nodesCount(G));
}

template<typename GraphTy>
typename LoopTreeBuilder<GraphTy>::LoopTreeTy
LoopTreeBuilder<GraphTy>::build(GraphTy& G) {
    init(G);

    auto domTreeBuilder = DominatorTreeBuilder<GraphTy>();
    m_domTree = domTreeBuilder.build(G);

    collectBackEdges(Traits::entry(G));
    populate(G);
    // for (auto& loop: m_arena) {
    //     loop.dump(std::cout);
    // }
    return finalize();
}

template<typename GraphTy>
typename LoopTreeBuilder<GraphTy>::LoopTreeTy
LoopTreeBuilder<GraphTy>::finalize() {
    auto res = LoopTreeTy{};
    if (!m_arena.size()) {
        return res;
    }
    res.m_arena = std::move(m_arena);
    auto* loop = &res.m_arena[0];
    while(loop->getOuter()) { loop = loop->getOuter(); }
    res.m_head = loop;
    return res;
}

template<typename GraphTy>
void LoopTreeBuilder<GraphTy>::appendLoopInfo(NodeTy header, NodeTy backEdgeSrc) {
    auto loopIt = m_loopsMap.find(header);
    if (loopIt == m_loopsMap.end()) {
        auto& new_loop = m_arena.emplace_back(header);
        m_loopsMap[header] = &new_loop;
    }

    auto isReducible = m_domTree.dominate(header, backEdgeSrc);
    auto& loop = m_loopsMap[header];
    loop->addReducibility(isReducible);
    loop->addBackEdge(backEdgeSrc);
}

template<typename GraphTy>
void LoopTreeBuilder<GraphTy>::collectBackEdges(NodeTy node) {
    m_colors[node] = Gcolor::GRAY;

    for(auto it = Traits::outEdgeBegin(node); it != Traits::outEdgeEnd(node); ++it) {
        NodeTy nextNode = *it;
        if(m_colors[nextNode] == Gcolor::GRAY) {
            appendLoopInfo(nextNode, node);
        } else if(m_colors[nextNode] != Gcolor::BLACK) {
            collectBackEdges(nextNode);
        }
    }

    m_colors[node] = Gcolor::BLACK;
    m_dfsNodes.push_back(node);
}

template<typename GraphTy>
void LoopTreeBuilder<GraphTy>::populate(GraphTy& G) {
    for(auto&& node: m_dfsNodes) {
        auto loopIt = m_loopsMap.find(node);
        if (loopIt == m_loopsMap.end()) {
            continue;
        }

        auto loop = loopIt->second;
        if (loop->isReducible()) {
            auto header = loop->getHeader();
            m_colors[header] = Gcolor::BLACK;
            for(auto backIt = loop->backEdgesBegin(); backIt != loop->backEdgesEnd(); ++backIt) {
                populateInner(loop, *backIt);
            }
            m_colors[header] = Gcolor::WHITE;
        } else {
            auto backEdgeSrc = loop->backEdgesBegin();
            auto backEdgeEnd = loop->backEdgesEnd();
            for(; backEdgeSrc != backEdgeEnd; ++backEdgeSrc) {
                auto backEdgeSrcLoop = m_loopsMap.find(*backEdgeSrc);
                if (backEdgeSrcLoop != m_loopsMap.end()) {
                    m_loopsMap[*backEdgeSrc] = loop;
                }
            }
        }
    }
}

template<typename GraphTy>
void LoopTreeBuilder<GraphTy>::populateInner(LoopTreeNodeTy* loop, NodeTy node) {
    if (m_colors[node] == Gcolor::BLACK) {
        return;
    }

    m_colors[node] = Gcolor::BLACK;
    auto nodeLoopIt = m_loopsMap.find(node);
    if (nodeLoopIt == m_loopsMap.end()) {
        loop->insertNode(node);
    } else if(loop != nodeLoopIt->second && nodeLoopIt->second->getOuter() == nullptr) {
        nodeLoopIt->second->setOuter(loop);
        loop->insertInnerLoop(nodeLoopIt->second);
    }

    auto predIt = Traits::inEdgeBegin(node);
    for(; predIt != Traits::inEdgeEnd(node); ++predIt) {
        populateInner(loop, *predIt);
    }
}

} // namespace jade
