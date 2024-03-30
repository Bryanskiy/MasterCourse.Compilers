#include "liveness.hh"
#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace jade;

void CheckLiveIntervals(LiveIn lhs, LiveIn rhs) {
  ASSERT_EQ(lhs.begin, rhs.begin);
  ASSERT_EQ(lhs.end, rhs.end);
}

TEST(Liveness, Check) { ASSERT_TRUE(true); }

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
TEST(Liveness, Main) {
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

  Liveness liveness(function);
  liveness.compute();

  // check linear numbers
  EXPECT_EQ(liveness.getLinearNumber(v0), 2);
  EXPECT_EQ(liveness.getLinearNumber(v1), 4);
  EXPECT_EQ(liveness.getLinearNumber(v2), 6);
  EXPECT_EQ(liveness.getLinearNumber(v3), 10);
  EXPECT_EQ(liveness.getLinearNumber(v4), 10);
  EXPECT_EQ(liveness.getLinearNumber(v5), 12);
  EXPECT_EQ(liveness.getLinearNumber(v7), 18);
  EXPECT_EQ(liveness.getLinearNumber(v8), 20);
  EXPECT_EQ(liveness.getLinearNumber(v9), 26);

  // check live intervals
  CheckLiveIntervals(liveness.getLiveInterval(bbs[0]), LiveIn{0, 10});
  CheckLiveIntervals(liveness.getLiveInterval(bbs[1]), LiveIn{10, 16});
  CheckLiveIntervals(liveness.getLiveInterval(bbs[2]), LiveIn{16, 24});
  CheckLiveIntervals(liveness.getLiveInterval(bbs[3]), LiveIn{24, 30});

  CheckLiveIntervals(liveness.getLiveInterval(v0), LiveIn{2, 24});
  CheckLiveIntervals(liveness.getLiveInterval(v1), LiveIn{4, 10});
  CheckLiveIntervals(liveness.getLiveInterval(v2), LiveIn{6, 26});
  CheckLiveIntervals(liveness.getLiveInterval(v3), LiveIn{10, 26});
  CheckLiveIntervals(liveness.getLiveInterval(v4), LiveIn{10, 20});
  CheckLiveIntervals(liveness.getLiveInterval(v5), LiveIn{12, 14});
  CheckLiveIntervals(liveness.getLiveInterval(v9), LiveIn{26, 28});
  CheckLiveIntervals(liveness.getLiveInterval(v7), LiveIn{18, 24});
  CheckLiveIntervals(liveness.getLiveInterval(v8), LiveIn{20, 24});
}
