#include "domTree.hh"

namespace jade {
bool dominate(DomTree<BasicBlocksGraph> &domTree, Instruction *lhs,
              Instruction *rhs) {
  if (lhs == rhs) {
    return true;
  }

  auto *lhsBB = lhs->getParent();
  auto *rhsBB = rhs->getParent();

  if (lhsBB != rhsBB) {
    return domTree.dominate(lhsBB, rhsBB);
  }

  auto *iit = lhs->next();
  while (iit) {
    if (iit == rhs)
      return true;
    iit = iit->next();
  }

  return false;
}
} // namespace jade
