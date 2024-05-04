#pragma once

#include "function.hh"
#include <memory>
#include <string_view>
#include <vector>
namespace jade {

struct Pass {
  virtual void run(Function *fn) {}
};

class PassManager {
public:
  PassManager(Function *fn) : m_fn(fn) {}

  void registerPass(std::unique_ptr<Pass> pass) {
    m_passes.emplace_back(std::move(pass));
  }
  void run() {
    for (auto &&pass : m_passes) {
      pass->run(m_fn);
    }
  }

private:
  Function *m_fn;
  std::vector<std::unique_ptr<Pass>> m_passes;
};

} // namespace jade
