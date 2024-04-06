#include "regAlloc.hh"
#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "liveness.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace jade;

void checkLocation(Location lhs, Location rhs) {
  ASSERT_EQ(lhs.idx, rhs.idx);
  ASSERT_EQ(lhs.on_stack, rhs.on_stack);
}

TEST(RegAlloc, Check) { ASSERT_TRUE(true); }

// func i32 test_lecture() {                | Lin num
// 0:                                       |   0
//   V0 = i32 1                             |   2
//   V1 = i32 10                            |   4
//   V2 = i32 20                            |   6
//   Jmp 1                                  |   8
//                                          |
// 1:                                       |   10
//   V3 = i32 Phi void 1, i32 V0, i32 V7    |   10
//   V4 = i32 Phi void 1, i32 V1, i32 V8    |   10
//   V5 = i1 Cmp EQ i32 V4, i32 V0          |   12
//   If i1 V5, T:2, F:3                     |   14
//                                          |
// 2:                                       |   16
//   V7 = i32 Mul i32 V3, i32 V4            |   18
//   V8 = i32 Sub i32 V4, i32 V0            |   20
//   Jmp 1                                  |   22
//                                          |
// 3:                                       |   24
//   V9 = i32 Add i32 V2, i32 V3            |   26
//   Ret void 4, i32 V9                     |   28
//
// }
TEST(RegAlloc, Main) {
  auto function = Function{};
  std::array<BasicBlock *, 4> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  // bb 0
  auto builder = InstrBulder{bbs[0]};
  auto v0 = builder.create<ConstI32>(1, "v0");
  auto v1 = builder.create<ConstI32>(10, "v1");
  auto v2 = builder.create<ConstI32>(20, "v2");
  builder.create<GotoInstr>(bbs[1], "goto1");

  // bb 1
  builder = InstrBulder{bbs[1]};
  auto v3 = builder.create<PhiInstr>(Type::create<Type::I32>(), "v3");
  auto v4 = builder.create<PhiInstr>(Type::create<Type::I32>(), "v4");
  auto v5 = builder.create<CmpInstr>(v4, v0, Opcode::EQ, "v5");
  builder.create<IfInstr>(v5, bbs[3], bbs[2], "if1");

  // bb 2
  builder = InstrBulder{bbs[2]};
  auto v7 = builder.create<BinaryOp>(v3, v4, Opcode::MUL, "v7");
  auto v8 = builder.create<BinaryOp>(v4, v0, Opcode::SUB, "v8");
  builder.create<GotoInstr>(bbs[1], "goto2");

  // bb3
  builder = InstrBulder{bbs[3]};
  auto v9 = builder.create<BinaryOp>(v2, v3, Opcode::ADD, "v9");
  builder.create<RetInstr>(v9, "ret1");

  v3->addOption(v0, bbs[0]);
  v3->addOption(v7, bbs[2]);
  v4->addOption(v1, bbs[0]);
  v4->addOption(v8, bbs[2]);

  RegAlloc<3> regAlloc{function};
  regAlloc.run();

  //  |  0|  2|  4|  6|  8| 10| 12| 14| 16| 18| 20| 22| 24| 26| 28|
  // 0|   | r0| r0| r0| r0| r0| r0| r0| r0| r0| r0| r0| r0|   |   |
  // 1|   |   | r1| r1| r1| r1|   |   |   |   |   |   |   |   |   |
  // 2|   |   |   | s1| s1| s1| s1| s1| s1| s1| s1| s1| s1| s1|   |
  // 3|   |   |   |   |   | s0| s0| s0| s0| s0| s0| s0| s0| s0|   |
  // 4|   |   |   |   |   | r1| r1| r1| r1| r1| r1|   |   |   |   |
  // 5|   |   |   |   |   |   | r2| r2|   |   |   |   |   |   |   |
  // 6|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // 7|   |   |   |   |   |   |   |   |   | r2| r2| r2| r2|   |   |
  // 8|   |   |   |   |   |   |   |   |   |   | r1| r1| r1|   |   |
  // 9|   |   |   |   |   |   |   |   |   |   |   |   |   | r1| r1|

  checkLocation(regAlloc.getLocation(v0), Location{0, false});
  checkLocation(regAlloc.getLocation(v1), Location{1, false});
  checkLocation(regAlloc.getLocation(v2), Location{1, true});
  checkLocation(regAlloc.getLocation(v3), Location{0, true});
  checkLocation(regAlloc.getLocation(v4), Location{1, false});
  checkLocation(regAlloc.getLocation(v5), Location{2, false});
  checkLocation(regAlloc.getLocation(v7), Location{2, false});
  checkLocation(regAlloc.getLocation(v8), Location{1, false});
  checkLocation(regAlloc.getLocation(v9), Location{1, false});
}
