#pragma once

#include <vector>
#include <memory>

#include "ilist.hh"
#include "instruction.hh"

namespace jade {

class BasicBlock final : public Value, public IListNode<BasicBlock> {
public:
private:
};

} // namespace jade
