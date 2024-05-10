#include "dce.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>

namespace jade {

void DCE::visitBB(BasicBlock *bb) {
  auto instr = &*bb->begin();
  while (instr) {
    if (instr->numUsers() == 0 && !instr->isTerm()) {
      auto *next = static_cast<Instruction *>(instr->getNext());
      bb->remove(instr);
      instr = next;
    } else {
      instr = static_cast<Instruction *>(instr->getNext());
    }
  }
}

} // namespace jade
