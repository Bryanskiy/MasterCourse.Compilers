#include "inline.hh"
#include "IR.hh"
#include "PM.hh"
#include "function.hh"
#include "graphs.hh"
#include "opcodes.hh"
#include "gtest/gtest.h"
#include <array>

using namespace jade;

// Caller graph
// bb0: {
//     v0: i64 = const 1;
//     v1: i64 = const 5;
//     goto -> bb1;
// }
// bb1: {
//     v2: i64 = add v0, v1;
//     v3: i64 = call func v2, v0;
//     v4: i64 = sub v3, v0;
//     ret v5;
// }
Function createCallerGraph(Function *callee) {
  auto function = Function{};

  auto bb0 = function.create<BasicBlock>();
  auto bb1 = function.create<BasicBlock>();

  auto v0 = bb0->create<ConstI64>(1, "v0");
  auto v1 = bb0->create<ConstI64>(5, "v1");
  bb0->create<GotoInstr>(bb1);

  auto v2 = bb1->create<BinaryOp>(v0, v1, Opcode::ADD, "v2");
  auto v3 = bb1->create<CallInstr>(callee, Type::create<Type::I64>(), "v3");
  v3->addArg(v2);
  v3->addArg(v0);
  auto v4 = bb1->create<BinaryOp>(v3, v0, Opcode::SUB, "v4");
  bb1->create<RetInstr>(v4);

  return function;
}

// Callee graph
// bb0: {
//     v0: i64 = param x;
//     v1: i64 = param y;
//     v2: i64 = const 1;
//     goto -> bb1;
// }
// bb1: {
//     v3: i1 = cmp v0, v1;
//     if (v3, bb2, bb3);
// }
//
// bb2: {
//     v4: i64 = add v0, v2;
//     ret v4;
// }
//
// bb3: {
//     v5: i64 = sub v1, v2;
//     ret v5;
// }
Function createCalleeGraph() {
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
  auto v3 = bb1->create<BinaryOp>(v0, v1, Opcode::EQ);
  auto if_ = bb1->create<IfInstr>(v3, bb2, bb3);

  // bb 2
  auto v4 = bb2->create<BinaryOp>(v0, v1, Opcode::ADD);
  bb2->create<RetInstr>(v4);

  // bb3
  auto v5 = bb3->create<BinaryOp>(v1, v2, Opcode::SUB);
  bb3->create<RetInstr>(v5);

  return function;
}

TEST(Inline, test1) {
  auto callee = createCalleeGraph();
  auto caller = createCallerGraph(&callee);

  auto pm = PassManager(&caller);
  pm.registerPass(std::make_unique<Inline>());
  pm.run();
  caller.dump(std::cout);
}
