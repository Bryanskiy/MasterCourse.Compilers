#pragma once

#include <set>
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
        m_visited.insert(node);
    }

    static DFSIterator begin(GraphTy& G) { return DFSIterator{GraphTy::entry(G)}; }
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

            while(EdgeIt != GraphTy::edgeEnd(currentNode)) {
                NodeTy nextNode = *EdgeIt;
                if(m_visited.find(nextNode) == m_visited.end()) {
                    m_stack.push_back(VE(nextNode, GraphTy::edgeBegin(nextNode)));
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

} // namespace jade
