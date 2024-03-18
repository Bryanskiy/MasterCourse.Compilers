#include "liveness.hh"
#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace jade;

TEST(Liveness, Check) { ASSERT_TRUE(true); }

// func i32 test_lecture() {
// 0:
//   V0 = i32 1
//   V1 = i32 10
//   V2 = i32 20
//   Jmp 1
//
// 1:
//   V3 = i32 Phi void 1, i32 V0, i32 V7
//   V4 = i32 Phi void 1, i32 V1, i32 V8
//   V5 = i1 Cmp EQ i32 V4, i32 V0
//   If i1 V5, T:2, F:3
//
// 2:
//   V7 = i32 Mul i32 V3, i32 V4
//   V8 = i32 Sub i32 V4, i32 V0
//   Jmp 1
//
// 3:
//   V9 = i32 Add i32 V2, i32 V3
//   Ret void 4, i32 V9
//
// }
TEST(Liveness, Main) {
  auto function = Function{};
  std::array<BasicBlock *, 4> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  // bb 0
  auto builder = InstrBulder{bbs[0]};
  auto v0 = builder.create<ConstI32>(1);
  auto v1 = builder.create<ConstI32>(10);
  auto v2 = builder.create<ConstI32>(20);
  builder.create<GotoInstr>(bbs[1]);

  // bb 1
  builder = InstrBulder{bbs[1]};
  auto v3 = builder.create<PhiInstr>(Type::create<Type::I32>());
  auto v4 = builder.create<PhiInstr>(Type::create<Type::I32>());
  auto v5 = builder.create<CmpInstr>(v4, v0, Opcode::EQ);
  builder.create<IfInstr>(v5, bbs[3], bbs[2]);

  // bb 2
  builder = InstrBulder{bbs[2]};
  auto v7 = builder.create<BinaryOp>(v3, v4, Opcode::MUL);
  auto v8 = builder.create<BinaryOp>(v4, v0, Opcode::SUB);
  builder.create<GotoInstr>(bbs[1]);

  // bb3
  builder = InstrBulder{bbs[3]};
  auto v9 = builder.create<BinaryOp>(v2, v3, Opcode::ADD);
  builder.create<RetInstr>(v9);

  v3->addOption(v0, bbs[0]);
  v3->addOption(v7, bbs[1]);
  v4->addOption(v1, bbs[0]);
  v4->addOption(v8, bbs[1]);

  Liveness liveness(function);
  liveness.compute();
  EXPECT_EQ(liveness.getLinearNumber(v0), 2);
  EXPECT_EQ(liveness.getLinearNumber(v1), 4);
  EXPECT_EQ(liveness.getLinearNumber(v2), 6);
  EXPECT_EQ(liveness.getLinearNumber(v3), 10);
  EXPECT_EQ(liveness.getLinearNumber(v4), 10);
  EXPECT_EQ(liveness.getLinearNumber(v5), 12);
  EXPECT_EQ(liveness.getLinearNumber(v7), 18);
  EXPECT_EQ(liveness.getLinearNumber(v8), 20);
  EXPECT_EQ(liveness.getLinearNumber(v9), 26);
}
