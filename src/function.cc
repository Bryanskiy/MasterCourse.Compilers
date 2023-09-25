#include "function.hh"
#include <memory>

namespace jade {

BasicBlock* Function::append() {
    return m_bbs.append();
}

} // namespace jade
