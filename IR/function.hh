#pragma once

#include <cassert>
#include <memory>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "IR.hh"
#include "graph.hh"
#include "ilist.hh"

namespace jade {

using BasicBlocks = IList<BasicBlock>;
using BasicBlocksRef = IList<BasicBlock, IListBorrower<BasicBlock>>;

class BasicBlocksGraph {
private:
  BasicBlocksRef m_bbs;
  std::unordered_map<BasicBlock *, Gcolor> m_colors;

public:
  BasicBlocksGraph() = default;
  BasicBlocksGraph(BasicBlocksRef c) : m_bbs{c} {}

  std::size_t size() const { return m_bbs.getLast()->getId() + 1; }
  auto nodes() const { return Range{m_bbs.begin(), m_bbs.end()}; }
};

template <> struct GraphTraits<BasicBlocksGraph> {
  using NodeTy = BasicBlock *;
  using EdgesItTy = decltype(BasicBlocksGraph()
                                 .nodes()
                                 .begin()
                                 .getPtr()
                                 ->successors()
                                 .begin());
  using NodesIt = decltype(BasicBlocksGraph().nodes().begin());

  static NodeTy entry(const BasicBlocksGraph &G) {
    return G.nodes().begin().getPtr();
  }
  static NodesIt nodesBegin(BasicBlocksGraph &G) { return G.nodes().begin(); }

  static NodesIt nodesEnd(BasicBlocksGraph &G) { return G.nodes().end(); }

  static std::size_t id(NodeTy node) { return node->getId(); }
  static std::size_t nodesCount(const BasicBlocksGraph &G) { return G.size(); }

  static EdgesItTy outEdgeBegin(NodeTy node) {
    return node->successors().begin();
  }
  static EdgesItTy outEdgeEnd(NodeTy node) { return node->successors().end(); }

  static EdgesItTy inEdgeBegin(NodeTy node) {
    return node->predecessors().begin();
  }
  static EdgesItTy inEdgeEnd(NodeTy node) { return node->predecessors().end(); }
};

class Function {
public:
  using iterator = IListIterator<BasicBlock>;

  template <typename T, typename... Args> T *create(Args &&...args);
  void insert(BasicBlock *bb);
  void remove(BasicBlock *bb) { m_bbs.remove(bb); }

  auto getBasicBlocks() { return BasicBlocksGraph(m_bbs.borrow()); }
  void dump(std::ostream &stream) {
    for (auto bb = m_bbs.begin(); bb != m_bbs.end(); ++bb) {
      stream << bb->getName() << ": " << std::endl;
      bb->dump(stream);
      stream << std::endl;
    }
  }

private:
  BasicBlocks m_bbs;
};

template <typename T, typename... Args> T *Function::create(Args &&...args) {
  assert(0);
}

template <> inline BasicBlock *Function::create<BasicBlock>(BasicBlock &bb) {
  auto *copy = new BasicBlock{bb};
  insert(copy);
  return copy;
}

template <> inline BasicBlock *Function::create<BasicBlock>() {
  auto *bb = new BasicBlock{};
  insert(bb);
  return bb;
}

} // namespace jade
