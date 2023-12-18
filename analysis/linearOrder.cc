#include "linearOrder.hh"
#include "IR.hh"
#include "graph.hh"
#include "opcodes.hh"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>

namespace jade {

std::vector<BasicBlock*> LinearOrder::linearize() {
    std::stack<BasicBlock*> stack;
    auto graph  = m_graph.getBasicBlocks();
    stack.push(Traits::entry(graph));
    m_visited.insert(Traits::entry(graph));

    while(!stack.empty()) {
        auto node = stack.top();
        stack.pop();
        m_visited.insert(node);
        m_linear.push_back(node);

        if (!m_skip_cond) {
            processCondition(node);
        }
        auto successors = node->collectSuccessors();
        for(auto succ = successors.begin(); succ != successors.end(); ++succ) {
            if(checkBlock(*succ)) {
                stack.push(*succ);
            }
        }
    }

    return m_linear;
}

void LinearOrder::processCondition(BasicBlock* bb) {
    auto successors = bb->collectSuccessors();
    auto terminator = bb->terminator();
    if(terminator->getOpcode() != Opcode::IF) {
        return;
    }

    auto trueBranch = successors[1];
    auto falseBranch = successors[0];

    if(m_visited.find(trueBranch) != m_visited.end() &&
        m_visited.find(falseBranch) != m_visited.end())
    {
        auto newBB = insertGotoBB(bb, falseBranch);
        m_linear.push_back(newBB);
    } else if (shouldInverseBranches(falseBranch, trueBranch, bb))
    {
        bb->inverseCondition();
    }

}

bool LinearOrder::shouldInverseBranches(BasicBlock* falseBranch, BasicBlock* trueBranch, BasicBlock* cond) {
    if(m_visited.find(trueBranch) != m_visited.end()) {
        return false;
    }

    return m_visited.find(falseBranch) == m_visited.end() &&
        m_loopTree.getLoop(falseBranch) == m_loopTree.getLoop(cond);
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

BasicBlock* LinearOrder::insertGotoBB(BasicBlock* source, BasicBlock* dst) {
    auto newBB = m_graph.create<BasicBlock>();
    auto builder = InstrBulder(newBB);
    builder.create<GotoInstr>(dst);
    m_graph.insertBetween(source, dst, newBB);
    return newBB;
}

} // namespace jade
