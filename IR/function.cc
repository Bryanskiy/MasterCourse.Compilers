#include "function.hh"

namespace jade {

void Function::insert(BasicBlock *bb) {
  std::size_t id = 0;
  if (!m_bbs.empty()) {
    id = m_bbs.getLast()->getId() + 1;
  }
  bb->setId(id);

  std::stringstream name;
  name << "bb" << id;
  bb->setName(name.str());

  m_bbs.push_back(bb);
}

} // namespace jade
