#include "checksElimination.hh"
#include "IR.hh"
#include "PM.hh"
#include "dce.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "liveness.hh"
#include "opcodes.hh"
#include "regAlloc.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace jade;

// bb0: {
//     v0: i64 = param x;
//     v1: i64 = param y;
//     v2: i64 = const 10;
//     goto -> bb1;
// }
// bb1: {
//     zeroCheck(v0)
//     v3: i64 = div v2, v0;
//     zeroCheck(v0);
//     v4: i64 = div v1, v0;
//     v5: i1 = cmp v3, v4;
//     if (v5, bb2, bb3);
// }
//
// bb2: {
//     zeroCheck(v0)
//     v6: i64 = add v0, v2;
//     zeroCheck(v6)
//     ret v6;
// }
//
// bb3: {
//     ret v1;
// }
TEST(checksElimination, zeroCheck) {
  auto function = Function{};

  auto bb0 = function.create<BasicBlock>();
  auto bb1 = function.create<BasicBlock>();
  auto bb2 = function.create<BasicBlock>();
  auto bb3 = function.create<BasicBlock>();

  // bb 0
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I64>());
  auto v1 = bb0->create<ParamInstr>(Type::create<Type::I64>());
  auto v2 = bb0->create<ConstI64>(1);
  bb0->create<GotoInstr>(bb1);

  // bb 1
  auto *zc0 = bb1->create<UnaryOp>(v0, Opcode::ZeroCheck);
  auto *v3 = bb1->create<BinaryOp>(v2, v0, Opcode::DIV);
  auto *zc1 = bb1->create<UnaryOp>(v0, Opcode::ZeroCheck);
  auto *v4 = bb1->create<BinaryOp>(v1, v0, Opcode::DIV);
  auto *v5 = bb1->create<BinaryOp>(v3, v4, Opcode::EQ);
  auto *if_ = bb1->create<IfInstr>(v5, bb2, bb3);

  // bb 2
  auto *zc2 = bb2->create<UnaryOp>(v0, Opcode::ZeroCheck);
  auto v6 = bb2->create<BinaryOp>(v0, v2, Opcode::ADD);
  auto *zc3 = bb2->create<UnaryOp>(v0, Opcode::ZeroCheck);
  bb2->create<RetInstr>(v6);

  // bb3
  bb3->create<RetInstr>(v0);

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ChecksElimination>());
  pm.run();

  // function.dump(std::cout);

  ASSERT_EQ(&*bb2->begin(), v6);
  ASSERT_EQ(v3->next(), v4);
  ASSERT_EQ(v3->prev(), zc0);
  ASSERT_EQ(v6->next(), zc3);
}

// bb0: {
//     v0: i64 = param x;
//     v1: i64 = param y;
//     v2: i64 = const 10;
//     goto -> bb1;
// }
// bb1: {
//     boundsCheck(v0, v2);
//     v3: i64 = add v1, v2;
//     v4: i1 = cmp v1, v3;
//     if (v5, bb2, bb3);
// }
//
// bb2: {
//     boundsCheck(v0, v2);
//     v5: i64 = add v3, v2;
//     ret v5;
// }
//
// bb3: {
//     v6: i64 = const 20;
//     boundsCheck(v0, v6);
//     v7: i64 = add v3, v6;
//     ret v7;
// }
TEST(checksElimination, boundsCheck) {
  auto function = Function{};

  auto bb0 = function.create<BasicBlock>();
  auto bb1 = function.create<BasicBlock>();
  auto bb2 = function.create<BasicBlock>();
  auto bb3 = function.create<BasicBlock>();

  // bb 0
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I64>());
  auto v1 = bb0->create<ParamInstr>(Type::create<Type::I64>());
  auto v2 = bb0->create<ConstI64>(1);
  bb0->create<GotoInstr>(bb1);

  // bb 1
  auto *bc0 = bb1->create<BinaryOp>(v0, v2, Opcode::BoundsCheck);
  auto *v3 = bb1->create<BinaryOp>(v1, v2, Opcode::ADD);
  auto *v4 = bb1->create<BinaryOp>(v1, v3, Opcode::EQ);
  auto *if_ = bb1->create<IfInstr>(v4, bb2, bb3);

  // bb 2
  auto *bc1 = bb2->create<BinaryOp>(v0, v2, Opcode::BoundsCheck);
  auto v5 = bb2->create<BinaryOp>(v3, v2, Opcode::ADD);
  bb2->create<RetInstr>(v5);

  // bb3
  auto *v6 = bb3->create<ConstI64>(1);
  auto *bc2 = bb3->create<BinaryOp>(v0, v6, Opcode::BoundsCheck);
  auto v7 = bb3->create<BinaryOp>(v3, v6, Opcode::ADD);
  bb3->create<RetInstr>(v7);

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ChecksElimination>());
  pm.run();

  // function.dump(std::cout);
  ASSERT_EQ(&*bb1->begin(), bc0);
  ASSERT_EQ(&*bb2->begin(), v5);
  ASSERT_EQ(v6->next(), bc2);
}
