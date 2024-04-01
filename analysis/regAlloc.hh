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
  void sortLiveIntervals();
  void expireOldIntervals(std::size_t i);
  void spillAtInterval(std::size_t i);

private:
  Function &m_func;
  std::vector<LiveIn> m_intervals;
  // sorted intervals
  std::vector<std::size_t> m_indicies;
  // map from interval idx to reg idx
  std::unordered_map<std::size_t, std::size_t> m_registers;

  using Comparator = std::function<bool(std::size_t, std::size_t)>;
  Comparator m_comparator = [this](std::size_t lhs, std::size_t rhs) {
    return this->m_intervals[lhs].end < this->m_intervals[rhs].end;
  };
  std::set<std::size_t, Comparator> m_activeIntervals{m_comparator};

  std::array<std::size_t, RegNum> m_regs;

public:
  RegAlloc(Function &func) : m_func(func) {}

  void run();
};

template <std::size_t RegNum> void RegAlloc<RegNum>::run() {
  sortLiveIntervals();
  for (auto &&idx : m_indicies) {
    expireOldIntervals(idx);

    if (m_activeIntervals.size() == RegNum) {
      spillAtInterval(idx);
    } else {
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
    m_registers.erase(*it);
  }
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::spillAtInterval(std::size_t i) {}

template <std::size_t RegNum> void RegAlloc<RegNum>::sortLiveIntervals() {
  Liveness livenessAnalyser{m_func};
  livenessAnalyser.compute();
  auto &&liveIntervals = livenessAnalyser.getLiveIntervals();

  m_intervals.reserve(liveIntervals.size());
  m_indicies.resize(liveIntervals.size());

  for (auto &&[_, interval] : liveIntervals) {
    m_intervals.push_back(interval);
  }

  std::iota(m_indicies.begin(), m_indicies.end(), 0);
  std::sort(m_indicies.begin(), m_indicies.end(),
            [this](std::size_t lhs, std::size_t rhs) {
              return this->m_intervals[lhs].begin <
                     this->m_intervals[rhs].begin;
            });
}

} /* namespace jade */
