#pragma once

#include "IR.hh"
#include "function.hh"
#include "liveness.hh"

#include <array>
#include <functional>
#include <numeric>
#include <ostream>
#include <unordered_map>
#include <vector>
namespace jade {

struct Location {
  std::size_t idx;
  bool on_stack;
};

template <std::size_t RegNum> class RegAlloc {
private:
  void init();
  void expireOldIntervals(std::size_t i);
  void spillAtInterval(std::size_t i);

  void dumpAllocInfo(std::ostream &out);

private:
  Function &m_func;

  std::vector<LiveIn> m_intervals;
  std::vector<Value *> m_instructions;
  // sorted intervals
  std::vector<std::size_t> m_indicies;

  class ActiveINtervals {
    std::list<std::size_t> m_data;
    const RegAlloc *m_regalloc;

  public:
    using iterator = decltype(m_data.begin());

    ActiveINtervals(const RegAlloc *regalloc) : m_regalloc{regalloc} {}

    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }
    auto rbegin() const { return m_data.rbegin(); }
    auto rend() const { return m_data.rend(); }

    std::size_t size() const { return m_data.size(); }

    auto erase(std::list<std::size_t>::iterator it) { return m_data.erase(it); }
    void erase(std::size_t val) { m_data.remove(val); }

    void insert(std::size_t idx) {
      m_data.push_back(idx);
      m_data.sort([this](std::size_t lhs, std::size_t rhs) {
        return m_regalloc->m_intervals[lhs].end <
               m_regalloc->m_intervals[rhs].end;
      });
    }

  } m_activeIntervals{this};

  std::vector<std::size_t> m_freeRegs;

  std::unordered_map<Value *, Location> m_allocInfo;
  std::size_t m_stackLocation;

public:
  RegAlloc(Function &func) : m_func(func) {}
  Location getLocation(Instruction *instr) { return m_allocInfo[instr]; }

  void run();
};

template <std::size_t RegNum> void RegAlloc<RegNum>::init() {
  m_stackLocation = 0;
  m_freeRegs.resize(RegNum);
  std::iota(m_freeRegs.rbegin(), m_freeRegs.rend(), 0);

  Liveness livenessAnalyser{m_func};
  livenessAnalyser.compute();
  auto &&liveIntervals = livenessAnalyser.getLiveIntervals();

  m_intervals.reserve(liveIntervals.size());
  m_instructions.reserve(liveIntervals.size());

  for (auto &&[value, interval] : liveIntervals) {
    if (value->is_vreg()) {
      m_intervals.push_back(interval);
      m_instructions.push_back(value);
    }
  }
  m_indicies.resize(m_intervals.size());

  std::iota(m_indicies.begin(), m_indicies.end(), 0);
  std::sort(m_indicies.begin(), m_indicies.end(),
            [this](std::size_t lhs, std::size_t rhs) {
              return this->m_intervals[lhs].begin <
                     this->m_intervals[rhs].begin;
            });
}

template <std::size_t RegNum> void RegAlloc<RegNum>::run() {
  init();
  for (std::size_t idx : m_indicies) {
    expireOldIntervals(idx);

    if (m_activeIntervals.size() == RegNum) {
      spillAtInterval(idx);
    } else {
      std::size_t freeReg = m_freeRegs.back();
      m_freeRegs.pop_back();

      m_activeIntervals.insert(idx);
      m_allocInfo[m_instructions[idx]] = Location{freeReg, false};
    }
  }
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::expireOldIntervals(std::size_t i) {
  for (auto &&it = m_activeIntervals.begin(); it != m_activeIntervals.end();) {
    auto idx = *it;

    if (m_intervals[idx].end > m_intervals[i].begin) {
      return;
    }

    auto freeReg = m_allocInfo[m_instructions[idx]];
    m_freeRegs.push_back(freeReg.idx);

    it = m_activeIntervals.erase(it);
  }
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::spillAtInterval(std::size_t i) {
  std::size_t spill = *m_activeIntervals.rbegin();
  auto &&iLocation = m_allocInfo[m_instructions[i]];

  if (m_intervals[spill].end > m_intervals[i].end) {
    auto &&spillLocation = m_allocInfo[m_instructions[spill]];

    iLocation.idx = spillLocation.idx;
    spillLocation.on_stack = true;
    spillLocation.idx = m_stackLocation;

    m_activeIntervals.erase(spill);
    m_activeIntervals.insert(i);
  } else {
    iLocation = Location{m_stackLocation, true};
  }
  ++m_stackLocation;
}

template <std::size_t RegNum>
void RegAlloc<RegNum>::dumpAllocInfo(std::ostream &out) {
  out << "alloc info: " << std::endl;
  for (auto &&[value, location] : m_allocInfo) {
    out << value->getName() << " location idx: " << location.idx
        << " on stask: " << location.on_stack << std::endl;
  }
}

} /* namespace jade */
