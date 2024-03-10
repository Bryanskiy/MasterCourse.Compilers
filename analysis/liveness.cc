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
  auto linOrder = LinearOrder(graph).linearize();

  std::size_t currentBBNum = 0;
  std::size_t step = 2;
  for (auto &&bb : linOrder) {
    std::size_t currentInstrNum = currentBBNum;
    for (auto &&instr : *bb) {
      if (instr.getOpcode() == Opcode::PHI) {
        ret[&instr] = currentBBNum;
      } else {
        ret[&instr] = currentInstrNum;
      }
      currentInstrNum += step;
    }

    currentBBNum += step;
  }
}

} /* namespace jade */
