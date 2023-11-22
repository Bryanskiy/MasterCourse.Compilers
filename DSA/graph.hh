#pragma once

#include <cstddef>
#include <set>
#include <vector>

namespace jade {

template <typename GraphTy>
struct GraphTraits {
    using NodeTy = typename GraphTy::NodeTy;
    using NodesIt = typename GraphTy::NodesIt;
    using EdgesItTy = typename GraphTy::EdgesItTy;

    static NodeTy entry(const GraphTy& G);
    static NodesIt nodesBegin(GraphTy& G);
    static NodesIt nodesEnd(GraphTy& G);

    static std::size_t nodesCount(const GraphTy& G);
    static std::size_t id(NodeTy node);


    static EdgesItTy inEdgeBegin(NodeTy node);
    static EdgesItTy inEdgeEnd(NodeTy node);

    static EdgesItTy outEdgeBegin(NodeTy node);
    static EdgesItTy outEdgeEnd(NodeTy node);
};

template<typename GraphTy>
class DFSIterator {
public:
    using Traits = GraphTraits<GraphTy>;

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Traits::NodeTy;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    using NodeTy = value_type;
    using EdgesItTy = typename Traits::EdgesItTy;
public:
    DFSIterator() = default;

    DFSIterator(value_type node) {
        m_stack.push_back(VE(node, Traits::outEdgeBegin(node)));
        m_visited.insert(node);
    }

    static DFSIterator begin(GraphTy& G) { return DFSIterator{Traits::entry(G)}; }
    static DFSIterator end(GraphTy& G) { return DFSIterator{}; }

    // Accessors
    reference operator*() { return m_stack.back().first; }
    pointer operator->() { return &*this; }

    // Comparison operators
    bool operator==(const DFSIterator &x) const {
        return m_stack == x.m_stack;
    }
    bool operator!=(const DFSIterator &x) const { return !(*this == x); }

    // Increment operators
    DFSIterator& operator++() {
        step();
        return *this;
    }

private:

    void step() {
        while(!m_stack.empty()) {
            NodeTy currentNode = m_stack.back().first;
            EdgesItTy EdgeIt = m_stack.back().second;

            while(EdgeIt != Traits::outEdgeEnd(currentNode)) {
                NodeTy nextNode = *EdgeIt;
                if(m_visited.find(nextNode) == m_visited.end()) {
                    m_stack.push_back(VE(nextNode, Traits::outEdgeBegin(nextNode)));
                    m_visited.insert(nextNode);
                    return;
                }
                ++EdgeIt;
            }

            m_stack.pop_back();
        }
    }

private:
    using VE = std::pair<NodeTy, EdgesItTy>;
    std::vector<VE> m_stack;
    std::set<NodeTy> m_visited;
};

template<typename GraphTy>
class PostOrderIterator {
public:
    using Traits = GraphTraits<GraphTy>;

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Traits::NodeTy;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    using NodeTy = value_type;
    using EdgesItTy = typename Traits::EdgesItTy;
public:
    PostOrderIterator() = default;

    PostOrderIterator(value_type node) {
        std::set<NodeTy> m_visited;
        walk(m_visited, node);
    }

    static PostOrderIterator begin(GraphTy& G) {
        return PostOrderIterator{Traits::entry(G)};
    }
    static PostOrderIterator end(GraphTy& G) {
        auto it = PostOrderIterator{};
        it.m_counter = Traits::nodesCount(G);
        return it;
    }

    // Accessors
    reference operator*() { return m_order[m_counter]; }
    pointer operator->() { return &m_order[m_counter]; }

    // Comparison operators
    bool operator==(const PostOrderIterator &x) const {
        return (m_counter == x.m_counter);
    }
    bool operator!=(const PostOrderIterator &x) const { return !(*this == x); }

    // Increment operators
    PostOrderIterator& operator++() {
        ++m_counter;
        return *this;
    }

private:
    std::vector<NodeTy> m_order;
    std::size_t m_counter{0};

    void walk(std::set<NodeTy>& visited, reference node) {
        visited.insert(node);
        for(auto it = Traits::outEdgeBegin(node); it != Traits::outEdgeEnd(node); ++it) {
            NodeTy nextNode = *it;
            if(visited.find(nextNode) == visited.end()) {
                walk(visited, nextNode);
            }
        }

        m_order.push_back(node);
    }
};

} // namespace jade
