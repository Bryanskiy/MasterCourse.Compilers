#include "function.hh"
#include <algorithm>

namespace jade {

void Function::insertBetween(BasicBlock* source, BasicBlock* dst, BasicBlock* bb) {
    source->removeSuccessor(dst);
    source->addSuccessor(bb);
    bb->addSuccessor(dst);
}

} // namespace jade
