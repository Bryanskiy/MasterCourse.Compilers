#include "peepholes.hh"
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

TEST(Peepholes, And1) {
  auto function = Function{};
  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ConstI32>(0xFFFF0000, "v0");
  auto v2 = bb0->create<BinaryOp>(v0, v0, Opcode::AND);
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();

  // And x, x -> x
  ASSERT_EQ(ret->getVal(), v0);
}

TEST(Peepholes, And2) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I32>());
  auto v1 = bb0->create<ConstI32>(0x0, "v0");
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::AND);
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();

  // And x, 0 -> const 0
  auto res = static_cast<ConstI32 *>(ret->getPrev());
  ASSERT_EQ(res->getValue(), 0);
}

TEST(Peepholes, Add1) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I32>());
  auto v1 = bb0->create<ConstI32>(0x0, "v0");
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::ADD);
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();

  // Add x, 0 -> x
  ASSERT_EQ(ret->getVal(), v0);
}

TEST(Peepholes, Add2) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I32>());
  auto v2 = bb0->create<BinaryOp>(v0, v0, Opcode::ADD);
  pm.run();

  // add V1, V1 -> shl V1, 1
  auto term = bb0->terminator();
  ASSERT_EQ(term->getOpcode(), Opcode::SHL);
  auto input = term->input(0);
  ASSERT_EQ(input, v0);
}

TEST(Peepholes, Ashr1) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I32>());
  auto v1 = bb0->create<ConstI32>(0x0, "v0");
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::ASHR);
  auto ret = bb0->create<RetInstr>(v2);
  pm.run();

  // Ashr x, 0 -> x
  ASSERT_EQ(ret->getVal(), v0);
}

TEST(Peepholes, Ashr2) {
  auto function = Function{};

  auto pm = PassManager(&function);
  pm.registerPass(std::make_unique<PeepHoles>());

  auto bb0 = function.create<BasicBlock>();
  auto v0 = bb0->create<ConstI32>(0xF);
  auto v1 = bb0->create<ConstI32>(0x2);
  auto v2 = bb0->create<BinaryOp>(v0, v1, Opcode::SHL);
  auto v3 = bb0->create<BinaryOp>(v2, v1, Opcode::ASHR);
  auto ret = bb0->create<RetInstr>(v3);
  pm.run();

  // v2 = Shl v0, v1
  // v3. AShr v2, v1
  // -->  v0
  ASSERT_EQ(ret->getVal(), v0);
  auto prev = static_cast<Instruction *>(ret->getPrev());
  ASSERT_EQ(prev->getOpcode(), Opcode::SHL);
}
