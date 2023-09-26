#pragma once

#include <memory>
#include <vector>

#include "IR.hh"
#include "ilist.hh"

namespace jade {

class Param : public Value, public IListNode<Param> {
public:
  Param(Type type) : Value{type} {}
};

class Function {
public:

  BasicBlock* appendBB() {
    return m_bbs.append<BasicBlock>();
  }

  Param* appendParam(Type type) {
    return m_params.append<Param>(type);
  }

private:
  IList<BasicBlock> m_bbs;
  IList<Param> m_params;
};

} // namespace jade
