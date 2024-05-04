#include "constFolding.hh"
#include "IR.hh"
#include "PM.hh"
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

TEST(ConstantFolder, Add) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<ConstantFolder>());

  auto bb0 = function.create<BasicBlock>();
  auto builder = InstrBulder{bb0};
  {
    auto v0 = builder.create<ConstI64>(1, "v0");
    auto v1 = builder.create<ConstI64>(10, "v1");
    auto v2 = builder.create<BinaryOp>(v0, v1, Opcode::ADD, "v3");
    pm.run();
    auto res = static_cast<ConstI64 *>(bb0->terminator());
    ASSERT_EQ(res->getValue(), 11);
  }
  {
    auto v3 = builder.create<ConstI32>(15, "v3");
    auto v4 = builder.create<ConstI32>(15, "v4");
    auto v5 = builder.create<BinaryOp>(v3, v4, Opcode::ADD, "v5");
    pm.run();
    auto res = static_cast<ConstI32 *>(bb0->terminator());
    ASSERT_EQ(res->getValue(), 30);
  }
}
