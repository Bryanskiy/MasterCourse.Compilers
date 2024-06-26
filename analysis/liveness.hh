#pragma once

#include "IR.hh"
#include "function.hh"
#include "graph.hh"
#include "loopAnalyser.hh"
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jade {

struct LiveIn {
  LiveIn() = default;
  LiveIn(std::size_t pbegin, std::size_t pend) : begin(pbegin), end(pend) {
    assert(pbegin < pend);
  }

  std::size_t begin{0};
  std::size_t end{0};
};

inline bool operator==(LiveIn lhs, LiveIn rhs) {
  return lhs.begin == rhs.begin && lhs.end == rhs.end;
}

inline std::ostream &operator<<(std::ostream &out, LiveIn lin) {
  out << lin.begin << " " << lin.end;
  return out;
}

class Liveness {
public:
  using Traits = GraphTraits<BasicBlocksGraph>;
  using LiveSet = std::unordered_set<Instruction *>;
  using LiveIntervals = std::unordered_map<Value *, LiveIn>;

  Liveness(Function &func) : m_func{func} {}

  void compute();

  std::size_t getLinearNumber(Instruction *instr) const {
    return m_linearNumbers.at(instr);
  }

  LiveIn getLiveInterval(Value *val) const { return m_liveInts.at(val); }
  LiveIntervals const &getLiveIntervals() const { return m_liveInts; }

private:
  using LinearNumbers = std::unordered_map<Instruction *, std::size_t>;
  using LiveSets = std::unordered_map<BasicBlock *, LiveSet>;
  using LoopAnalyser = LoopTree<BasicBlocksGraph>;

  Function &m_func;
  LinearNumbers m_linearNumbers;
  LiveSets m_liveSets;
  LiveIntervals m_liveInts;
  LoopAnalyser m_loops;

  LinearNumbers computeLinearNumbers();
  LiveSet computeInitialLiveSet(BasicBlock *bb);
  void processEachInstr(LiveSet &live, BasicBlock *bb);
  void processLoop(LiveSet &live, BasicBlock *bb);
};

} // namespace jade
