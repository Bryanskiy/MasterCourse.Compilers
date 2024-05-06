#include "peepholes.hh"
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

TEST(Peepholes, And1) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto builder = InstrBulder{bb0};
  auto v0 = builder.create<ConstI32>(0xFFFF0000, "v0");
  auto v2 = builder.create<BinaryOp>(v0, v0, Opcode::AND);
  auto ret = builder.create<RetInstr>(v2);
  pm.run();

  // And x, x -> x
  ASSERT_EQ(ret->getVal(), v0);
}

TEST(Peepholes, And2) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto builder = InstrBulder{bb0};
  auto v0 = function.create<Param>(Type::create<Type::I32>());
  auto v1 = builder.create<ConstI32>(0x0, "v0");
  auto v2 = builder.create<BinaryOp>(v0, v1, Opcode::AND);
  auto ret = builder.create<RetInstr>(v2);
  pm.run();

  // And x, 0 -> const 0
  auto res = static_cast<ConstI32 *>(ret->getPrev());
  ASSERT_EQ(res->getValue(), 0);
}
