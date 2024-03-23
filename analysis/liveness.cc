#include "liveness.hh"
#include "linearOrder.hh"
#include "opcodes.hh"
#include <algorithm>
#include <cassert>

namespace jade {

void Liveness::compute() {
  auto graph = m_func.getBasicBlocks();

  auto loopBuilder = LoopTreeBuilder<BasicBlocksGraph>();
  m_loops = loopBuilder.build(graph);
  m_linearNumbers = computeLinearNumbers();
  auto linearOrder = LinearOrder(graph).linearize();
  return;

  for (auto &&bbIt = linearOrder.rbegin(), itEnd = linearOrder.rend();
       bbIt != itEnd; ++bbIt) {
    auto bb = *bbIt;
    auto live = computeInitialLiveSet(bb);

    // initial live interaval for instrs
    auto currInter = m_liveInts[bb];
    for (auto *instr : live) {
      m_liveInts[instr].begin =
          std::min(currInter.begin, m_liveInts[instr].begin);
      m_liveInts[instr].end = std::max(currInter.end, m_liveInts[instr].end);
    }

    // process each instr
    for (auto instrIt = bb->rbegin(); instrIt != bb->rend(); ++instrIt) {
      auto instr = instrIt.base().getPtr();

      // process output
      m_liveInts[instr].begin = m_linearNumbers[instr];
      live.erase(instr);

      // process inputs
      for (auto &&instrInputIt = instr->begin(); instrInputIt != instr->end();
           ++instrInputIt) {
        auto *input = *instrInputIt;
        m_liveInts[input].begin =
            std::min(currInter.begin, m_liveInts[input].begin);
        m_liveInts[input].end = std::max(currInter.end, m_liveInts[input].end);
        live.insert(input);
      }
    }

    // remove phi's
    for (auto *phi : bb->phis()) {
      if (live.count(phi)) {
        live.erase(phi);
      }
    }

    // process loop
    auto loop = m_loops.getLoop(bb);
    if (loop && loop->getHeader() == bb) {
      std::size_t loopEnd = 0;
      for (auto *block : loop->getNodes()) {
        loopEnd = std::max(m_liveInts[block].end, loopEnd);
      }

      for (auto *vreg : live) {
        m_liveInts[vreg].begin =
            std::min(currInter.begin, m_liveInts[vreg].begin);
        m_liveInts[vreg].end = loopEnd;
      }
    }
  }
}

Liveness::LiveSet Liveness::computeInitialLiveSet(BasicBlock *bb) {
  LiveSet live;
  auto successors = bb->successors();
  for (auto *succ : successors) {
    auto liveSet = m_liveSets[succ];
    for (auto *vreg : liveSet) {
      live.insert(vreg);
    }

    for (auto *phi : succ->phis()) {
      for (auto &&arg : *phi) {
        if (arg.first == bb) {
          live.insert(arg.second);
        }
      }
    }
  }

  return live;
}

Liveness::LinearNumbers Liveness::computeLinearNumbers() {
  auto ret = LinearNumbers{};
  auto graph = m_func.getBasicBlocks();
  auto linOrder = LinearOrder(graph);
  auto linearOrder = linOrder.linearize();

  std::size_t step = 2;
  std::size_t currentBBLive = 0;
  for (auto &&bb : linearOrder) {
    std::size_t currentInstLive = currentBBLive + step;
    for (auto &&instr : *bb) {

      if (instr.getOpcode() != Opcode::PHI) {
        ret[&instr] = currentInstLive;
        currentInstLive += step;
      } else {
        ret[&instr] = currentBBLive;
      }
    }
    m_liveInts[bb] = {currentBBLive, currentInstLive};
    currentBBLive = currentInstLive;
  }
  return ret;
}

} /* namespace jade */
