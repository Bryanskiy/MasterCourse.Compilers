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

TEST(ConstantFolder, Check) { ASSERT_TRUE(true); }

TEST(ConstantFolder, AddI64) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());
  pm.registerPass(std::make_unique<DCE>());

  auto bb0 = function.create<BasicBlock>();
  auto builder = InstrBulder{bb0};
  auto v0 = builder.create<ConstI64>(1, "v0");
  auto v1 = builder.create<ConstI64>(10, "v1");
  auto v2 = builder.create<BinaryOp>(v0, v1, Opcode::ADD, "v3");
  auto ret = builder.create<RetInstr>(v2);
  pm.run();
  auto res = static_cast<ConstI64 *>(&*bb0->begin());
  ASSERT_EQ(res->getValue(), 11);
}

TEST(ConstantFolder, AddI32) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());
  pm.registerPass(std::make_unique<DCE>());

  auto bb0 = function.create<BasicBlock>();
  auto builder = InstrBulder{bb0};
  auto v0 = builder.create<ConstI32>(1, "v0");
  auto v1 = builder.create<ConstI32>(10, "v1");
  auto v2 = builder.create<BinaryOp>(v0, v1, Opcode::ADD, "v3");
  auto ret = builder.create<RetInstr>(v2);
  pm.run();
  auto res = static_cast<ConstI32 *>(&*bb0->begin());
  ASSERT_EQ(res->getValue(), 11);
}
