#pragma once

#include <vector>

#include "graph.hh"

namespace jade {

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
        m_stack.push_back(VE(node, GraphTy::edgeBegin(node)));
    }

    static DFSIterator begin(GraphTy& G) { return DFSIterator{GraphTy::entry(G)}; }
    static DFSIterator end(GraphTy& G) { return DFSIterator{}; }

    // Accessors
    reference operator*() const { return m_stack.back().first; }
    pointer operator->() const { return &*this; }

    // Comparison operators
    bool operator==(const DFSIterator &x) const {
        return m_stack == x.m_stack;
    }
    bool operator!=(const DFSIterator &x) const { return !(*this == x); }

    // Increment operators
    DFSIterator& operator++() {

        return *this;
    }

private:

    void step() {
        NodeTy currentNode = m_stack.back().first;
        EdgesItTy currentEdge = m_stack.back().second;

        // TODO
    }

private:
    using VE = std::pair<typename Traits::NodeTy, typename Traits::EdgesItTy>;
    std::vector<VE> m_stack;
};

} // namespace jade
