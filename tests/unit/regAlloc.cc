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
}
