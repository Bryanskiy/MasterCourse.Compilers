#include "constFolding.hh"
#include "IR.hh"
#include "PM.hh"
#include "dce.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "liveness.hh"
#include "regAlloc.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace jade;

TEST(ConstantFolder, AddI64) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());
  pm.registerPass(std::make_unique<DCE>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ConstI64>(1, "v0");
  auto v1 = bb0->create<ConstI64>(10, "v1");
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::ADD, "v3");
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();
  auto res = static_cast<ConstI64 *>(&*bb0->begin());
  ASSERT_EQ(res->getValue(), 11);
}

TEST(ConstantFolder, MulI32) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());
  pm.registerPass(std::make_unique<DCE>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ConstI32>(3, "v0");
  auto v1 = bb0->create<ConstI32>(10, "v1");
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::MUL, "v3");
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();
  auto res = static_cast<ConstI32 *>(&*bb0->begin());
  ASSERT_EQ(res->getValue(), 30);
}

TEST(ConstantFolder, NegI16) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());
  pm.registerPass(std::make_unique<DCE>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ConstI16>(1, "v0");
  auto v1 = bb0->create<UnaryOp>(v0, Opcode::NEG, "v1");
  auto ret = bb0->create<RetInstr>(v1);
  pm.run();
  auto res = static_cast<ConstI16 *>(&*bb0->begin());
  ASSERT_EQ(res->getValue(), -1);
}
