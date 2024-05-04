#include "dce.hh"
#include "IR.hh"
#include "opcodes.hh"
#include <cassert>
#include <cstdint>

namespace jade {

void DCE::visitBB(BasicBlock *bb) {
  auto instr = &*bb->begin();
  auto builder = InstrBulder(bb);
  while (instr) {
    if (instr->numUsers() == 0 && !instr->isTerm()) {
      auto *next = static_cast<Instruction *>(instr->getNext());
      builder.remove(instr);
      instr = next;
    } else {
      instr = static_cast<Instruction *>(instr->getNext());
    }
  }
}

} // namespace jade
