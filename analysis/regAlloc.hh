#pragma once

#include "IR.hh"
#include "function.hh"
#include "liveness.hh"

#include <array>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <vector>
namespace jade {

template <std::size_t RegNum> class RegAlloc {
private:
  void init();
  void expireOldIntervals(std::size_t i);
  void spillAtInterval(std::size_t i);

private:
  Function &m_func;

  std::vector<LiveIn> m_intervals;
  std::vector<Value *> m_instructions;
  // sorted intervals
  std::vector<std::size_t> m_indicies;
  // map from interval idx to reg idx
  std::unordered_map<std::size_t, std::size_t> m_registers;

  using Comparator = std::function<bool(std::size_t, std::size_t)>;
  Comparator m_comparator = [this](std::size_t lhs, std::size_t rhs) {
    return this->m_intervals[lhs].end < this->m_intervals[rhs].end;
  };
  std::set<std::size_t, Comparator> m_activeIntervals{m_comparator};

  std::vector<std::size_t> m_freeRegs;

  struct Location {
    std::size_t idx;
    bool on_stack;
  };
  std::unordered_map<Value *, Location> m_allocInfo;
  std::size_t m_stackLocation;

public:
  RegAlloc(Function &func) : m_func(func) {}

  void run();
};

template <std::size_t RegNum> void RegAlloc<RegNum>::init() {
  m_stackLocation = 0;
  m_freeRegs.resize(RegNum);
  std::iota(m_freeRegs.begin(), m_freeRegs.end(), 0);

  Liveness livenessAnalyser{m_func};
  livenessAnalyser.compute();
  auto &&liveIntervals = livenessAnalyser.getLiveIntervals();

  m_intervals.reserve(liveIntervals.size());
  m_instructions.reserve(liveIntervals.size());
  m_indicies.resize(liveIntervals.size());

  for (auto &&[value, interval] : liveIntervals) {
    m_intervals.push_back(interval);
    m_instructions.push_back(value);
  }

  std::iota(m_indicies.begin(), m_indicies.end(), 0);
  std::sort(m_indicies.begin(), m_indicies.end(),
            [this](std::size_t lhs, std::size_t rhs) {
              return this->m_intervals[lhs].begin <
                     this->m_intervals[rhs].begin;
            });
}

template <std::size_t RegNum> void RegAlloc<RegNum>::run() {
  init();
  for (auto &&idx : m_indicies) {
    expireOldIntervals(idx);

    if (m_activeIntervals.size() == RegNum) {
      spillAtInterval(idx);
    } else {
      auto &&freeReg = m_freeRegs.back();
      m_freeRegs.pop_back();

      m_allocInfo[m_instructions[idx]] = Location{freeReg, false};
      m_activeIntervals.insert(idx);
    }
  }
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::expireOldIntervals(std::size_t i) {
  for (auto &&it = m_activeIntervals.begin(); it != m_activeIntervals.end();
       ++it) {
    if (m_intervals[*it].end > m_intervals[i].begin) {
      return;
    }

    m_activeIntervals.erase(it);
    auto freeReg = m_allocInfo[m_instructions[*it]];
    m_freeRegs.push_back(freeReg.idx);
  }
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::spillAtInterval(std::size_t i) {
  auto &&spill = *m_activeIntervals.rend();
  auto &&iLocation = m_allocInfo[m_instructions[i]];

  if (m_intervals[spill].end > m_intervals[i].end) {
    auto &&spillLocation = m_allocInfo[m_instructions[spill]];

    iLocation.idx = spillLocation.idx;
    spillLocation.on_stack = true;
    spillLocation.idx = m_stackLocation;

    m_activeIntervals.erase(spill);
  } else {
    iLocation = Location{m_stackLocation, true};
  }
  ++m_stackLocation;
}

} /* namespace jade */
