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
//   Jmp 1
//
// 1:
//   V4 = i32 Phi void 1, i32 V1, i32 V8
//   V3 = i32 Phi void 1, i32 V0, i32 V7
//   V5 = i1 CmpEQ i32 V4, i32 V0
//   If i1 V5, T:2, F:3
//
// 2:
//   V8 = i32 Sub i32 V4, i32 V0
//   V7 = i32 Mul i32 V3, i32 V4
//   Jmp 1
//
// 3:
//   Jmp 4
//
// 4:
//   V2 = i32 20
//   V9 = i32 Add i32 V2, i32 V3
//   Ret void 4, i32 V9
//
// }
TEST(Liveness, Main) {
  auto function = Function{};
  std::array<BasicBlock *, 5> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  // bb 0
  auto builder = InstrBulder{bbs[0]};
  auto v0 = builder.create<ConstI32>(1);
  auto v1 = builder.create<ConstI32>(10);
  builder.create<GotoInstr>(bbs[1]);

  // bb 1
  builder = InstrBulder{bbs[1]};
  auto v3 = builder.create<PhiInstr>(Type::create<Type::I32>());
  auto v4 = builder.create<PhiInstr>(Type::create<Type::I32>());
  auto v5 = builder.create<CmpInstr>(static_cast<Value *>(v4),
                                     static_cast<Value *>(v0), Opcode::EQ);
  builder.create<IfInstr>(static_cast<Value *>(v5), bbs[3], bbs[2]);

  // bb 2
  builder = InstrBulder{bbs[2]};
  auto v8 = builder.create<BinaryOp>(static_cast<Value *>(v4),
                                     static_cast<Value *>(v0), Opcode::SUB);
  auto v7 = builder.create<BinaryOp>(static_cast<Value *>(v3),
                                     static_cast<Value *>(v4), Opcode::MUL);
  builder.create<GotoInstr>(bbs[1]);

  // bb3
  builder = InstrBulder{bbs[3]};
  builder.create<GotoInstr>(bbs[4]);

  // bb4
  builder = InstrBulder{bbs[4]};
  auto v2 = builder.create<ConstI32>(20);
  auto v9 = builder.create<BinaryOp>(static_cast<Value *>(v2),
                                     static_cast<Value *>(v3), Opcode::ADD);
  builder.create<RetInstr>(v9);
}
