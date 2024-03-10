#include "liveness.hh"
#include "linearOrder.hh"
#include "opcodes.hh"

namespace jade {

void Liveness::compute() {
  m_linearNumbers = computeLinearNumbers();
  /* TODO */
}

Liveness::LinearNumbers Liveness::computeLinearNumbers() {
  auto ret = LinearNumbers{};
  auto graph = m_func.getBasicBlocks();
  auto linOrder = LinearOrder(graph);
  auto linearOrder = linOrder.linearize();

  std::size_t currentBBNum = 0;
  std::size_t step = 2;
  for (auto &&bb : linearOrder) {
    for (auto &&phi : bb->phis()) {
      ret[phi] = currentBBNum * step;
    }
    std::size_t currentInstrNum = currentBBNum;
    for (auto &&instr : *bb) {
      if (instr.getOpcode() != Opcode::PHI) {
        currentInstrNum += 1;
        ret[&instr] = currentInstrNum * step;
      }
    }
    currentBBNum = currentInstrNum + 1;
  }
  return ret;
}

} /* namespace jade */
