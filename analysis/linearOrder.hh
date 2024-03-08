#pragma once

#include <set>
#include <stack>
#include <unordered_set>
#include <vector>

#include "IR.hh"
#include "function.hh"
#include "graph.hh"
#include "loopAnalyser.hh"

namespace jade {

template <typename GraphTy> class LinearOrder {
public:
  using Traits = GraphTraits<GraphTy>;

  LinearOrder(GraphTy &G) : m_graph(G) {
    auto lBuilder = LoopTreeBuilder<GraphTy>();
    m_loopTree = lBuilder.build(G);
  }

  std::vector<typename Traits::NodeTy> linearize();

private:
  GraphTy &m_graph;
  LoopTree<GraphTy> m_loopTree;
  std::set<typename Traits::NodeTy> m_visited;
  std::vector<typename Traits::NodeTy> m_linear{};
};

template <typename GraphTy>
std::vector<typename GraphTraits<GraphTy>::NodeTy>
LinearOrder<GraphTy>::linearize() {
  auto rpoIt = RPOIterator<GraphTy>::begin(m_graph);
  auto rpoEnd = RPOIterator<GraphTy>::end(m_graph);

  for (; rpoIt != rpoEnd; ++rpoIt) {
    auto node = *rpoIt;
    if (m_visited.count(node)) {
      continue;
    }

    m_visited.insert(node);
    m_linear.push_back(node);

    auto *loop = m_loopTree.getLoop(node);
    if (loop && loop->getHeader() == node && loop->isReducible()) {
      auto loop_nodes = loop->getNodes();
      for (auto &&loop_node : loop_nodes) {
        if (m_visited.count(node)) {
          continue;
        }

        m_visited.insert(loop_node);
        m_linear.push_back(node);
      }
    }
  }

  return m_linear;
}

} // namespace jade
