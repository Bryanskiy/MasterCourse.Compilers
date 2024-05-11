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
  auto *zc2 = bb1->create<UnaryOp>(v0, Opcode::ZeroCheck);
  auto v6 = bb2->create<BinaryOp>(v0, v2, Opcode::ADD, "vv4");
  bb2->create<RetInstr>(v6);

  // bb3
  bb3->create<RetInstr>(v0);

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<CheckElimination>());
  pm.run();

  // function.dump(std::cout);

  ASSERT_EQ(&*bb2->begin(), v6);
  ASSERT_EQ(v3->next(), v4);
  ASSERT_EQ(v3->prev(), zc0);
}

TEST(checksElimination, boundsCheck) {}
