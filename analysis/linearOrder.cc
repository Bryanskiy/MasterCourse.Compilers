#include "linearOrder.hh"
#include "IR.hh"
#include "graph.hh"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>

namespace jade {

std::vector<BasicBlock*> LinearOrder::linearize() {
    std::stack<BasicBlock*> stack;
    stack.push(Traits::entry(m_graph));
    m_visited.insert(Traits::entry(m_graph));

    while(!stack.empty()) {
        auto node = stack.top();
        stack.pop();

        m_linear.push_back(node);

        for(auto succ = Traits::outEdgeBegin(node); succ != Traits::outEdgeEnd(node); ++succ) {
            if(checkBlock(*succ)) {
                stack.push(*succ);
                m_visited.insert(*succ);
            }
        }
    }

    return m_linear;
}

// is it possible to visit the node (constraints are correct)
bool LinearOrder::checkBlock(BasicBlock* bb) {
    auto checkPredicates = [this](BasicBlock* node, auto predicate) -> bool {
        auto begin = Traits::inEdgeBegin(node);
        auto end = Traits::inEdgeEnd(node);
        return std::all_of(begin, end, [this, &predicate](BasicBlock* pred) {
            return predicate(pred);
        });
    };

    if(m_visited.find(bb) != m_visited.end()) {
        return false;
    }

    auto* loop = m_loopTree.getLoop(bb);
    if(loop && loop->getHeader() == bb) {
        if(!loop->isReducible()) {
            return false;
        }
        return checkPredicates(loop->getHeader(), [this, loop](BasicBlock* node) -> bool {
            return m_visited.find(node) != m_visited.end() || loop->contains(node);
        });
    }

    return checkPredicates(bb, [this](BasicBlock* node) -> bool {
        return m_visited.find(node) != m_visited.end();
    });
}

} // namespace jade
