#pragma once

#include <memory>
#include <vector>

#include "IR.hh"
#include "ilist.hh"

namespace jade {

class Function {
public:

  BasicBlock* append();

private:
  IList<BasicBlock> m_bbs;
};
} // namespace jade
