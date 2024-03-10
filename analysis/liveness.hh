#pragma once

#include "IR.hh"
#include "function.hh"
#include "graph.hh"
#include <unordered_map>

namespace jade {

class Liveness {
public:
  using Traits = GraphTraits<BasicBlocksGraph>;

  Liveness(Function &func) : m_func{func} {}

  void compute();

  std::size_t getLinerNumber(Instruction *instr) const {
    return m_linearNumbers.at(instr);
  }

private:
  using LinearNumbers = std::unordered_map<Instruction *, std::size_t>;

  Function &m_func;
  LinearNumbers m_linearNumbers;

  LinearNumbers computeLinearNumbers();
};

} // namespace jade
