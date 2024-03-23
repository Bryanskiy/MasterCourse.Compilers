#include "liveness.hh"
#include "linearOrder.hh"
#include "opcodes.hh"
#include <algorithm>
#include <cassert>

namespace jade {

static void dumpLiveSet(std::ostream& stream, const Liveness::LiveSet& set) {
  for(auto* instr: set) {
    stream << instr->getName() << " ";
  }
  stream << std::endl;
}

void Liveness::compute() {
  auto graph = m_func.getBasicBlocks();

  auto loopBuilder = LoopTreeBuilder<BasicBlocksGraph>();
  m_loops = loopBuilder.build(graph);
  m_linearNumbers = computeLinearNumbers();
  auto linearOrder = LinearOrder(graph).linearize();

  for (auto &&bbIt = linearOrder.rbegin(), itEnd = linearOrder.rend();
       bbIt != itEnd; ++bbIt) {
    auto bb = *bbIt;
    std::cout << "Processing bb: " << bb->getName() << std::endl;
    auto live = computeInitialLiveSet(bb);

    std::cout << "Initial live set for " << bb->getName() << std::endl;
    dumpLiveSet(std::cout, live);

    // initial live interaval for instrs
    auto currInter = m_liveInts[bb];
    for (auto *instr : live) {
      m_liveInts[instr].begin =
          std::min(currInter.begin, m_liveInts[instr].begin);
      m_liveInts[instr].end = std::max(currInter.end, m_liveInts[instr].end);
    }

    // process each instr
    // FIXME: fix reverse iterator for ilist
    Instruction* instr = bb->terminator();
    while(instr != nullptr) {
    std::cout << "Processing instr: " << instr->getName() << std::endl;

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

      instr = static_cast<Instruction*>(instr->getPrev());
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

    m_liveSets[bb] = live;
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
