#include "function.hh"
#include <memory>

namespace jade {

BasicBlock* Function::append() {
    return m_bbs.append<BasicBlock>();
}

} // namespace jade
