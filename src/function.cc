#include "function.hh"
#include "basicBlock.hh"
#include <memory>

namespace jade {

BasicBlock* Function::append() {
    return m_bbs.append();
}

} // namespace jade
