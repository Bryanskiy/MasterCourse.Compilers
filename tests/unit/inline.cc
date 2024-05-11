#include "inline.hh"
#include "IR.hh"
#include "PM.hh"
#include "function.hh"
#include "graphs.hh"
#include "opcodes.hh"
#include "gtest/gtest.h"
#include <array>
#include <memory>
#include <vector>

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
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I64>(), "vv0");
  auto v1 = bb0->create<ParamInstr>(Type::create<Type::I64>(), "vv1");
  auto v2 = bb0->create<ConstI64>(1, "vv2");
  bb0->create<GotoInstr>(bb1);

  // bb 1
  auto v3 = bb1->create<BinaryOp>(v0, v1, Opcode::EQ, "vv3");
  auto if_ = bb1->create<IfInstr>(v3, bb2, bb3);

  // bb 2
  auto v4 = bb2->create<BinaryOp>(v0, v2, Opcode::ADD, "vv4");
  bb2->create<RetInstr>(v4, "retbb2");

  // bb3
  auto v5 = bb3->create<BinaryOp>(v1, v2, Opcode::SUB, "vv5");
  bb3->create<RetInstr>(v5, "retbb3");

  return function;
}

TEST(Inline, test1) {
  auto callee = createCalleeGraph();
  auto caller = createCallerGraph(&callee);

  auto pm = PassManager(&caller);
  pm.registerPass(std::make_unique<Inline>());
  pm.run();

  // caller.dump(std::cout);

  auto bbGraph = caller.getBasicBlocks().nodes();

  std::vector<BasicBlock *> bbs;
  for (auto bb = bbGraph.begin(); bb != bbGraph.end(); ++bb) {
    bbs.push_back(&*bb);
  }

  // bb0
  // v0: i64 = const 1;
  auto *v0 = &*bbs[0]->begin();
  ASSERT_EQ(v0->getOpcode(), Opcode::CONST);
  ASSERT_EQ(static_cast<ConstI64 *>(v0)->getValue(), 1);

  // v1: i64 = const 5;
  auto *v1 = v0->next();
  ASSERT_EQ(v1->getOpcode(), Opcode::CONST);
  ASSERT_EQ(static_cast<ConstI64 *>(v1)->getValue(), 5);

  // goto -> bb1;
  auto *gotoInstr = v1->next();
  ASSERT_EQ(gotoInstr->getOpcode(), Opcode::GOTO);
  ASSERT_EQ(static_cast<GotoInstr *>(gotoInstr)->getBB(), bbs[1]);

  // bb1
  auto *v2 = &*bbs[1]->begin();
  ASSERT_EQ(v2->getOpcode(), Opcode::ADD);
  ASSERT_EQ(v2->input(0), v0);
  ASSERT_EQ(v2->input(1), v1);

  auto *v3 = v2->next();
  ASSERT_EQ(v3->getOpcode(), Opcode::CONST);
  ASSERT_EQ(static_cast<ConstI64 *>(v3)->getValue(), 1);

  gotoInstr = v3->next();
  ASSERT_EQ(gotoInstr->getOpcode(), Opcode::GOTO);
  ASSERT_EQ(static_cast<GotoInstr *>(gotoInstr)->getBB(), bbs[3]);

  // bb4
  auto *v4 = &*bbs[4]->begin();
  ASSERT_EQ(v4->getOpcode(), Opcode::ADD);
  ASSERT_EQ(v4->input(0), v2);
  ASSERT_EQ(v4->input(1), v3);

  gotoInstr = v4->next();
  ASSERT_EQ(gotoInstr->getOpcode(), Opcode::GOTO);
  ASSERT_EQ(static_cast<GotoInstr *>(gotoInstr)->getBB(), bbs[2]);

  // bb5
  auto *v5 = &*bbs[5]->begin();
  ASSERT_EQ(v5->getOpcode(), Opcode::SUB);
  ASSERT_EQ(v5->input(0), v0);
  ASSERT_EQ(v5->input(1), v3);

  gotoInstr = v5->next();
  ASSERT_EQ(gotoInstr->getOpcode(), Opcode::GOTO);
  ASSERT_EQ(static_cast<GotoInstr *>(gotoInstr)->getBB(), bbs[2]);

  // bb2
  auto *v6 = &*bbs[2]->begin();
  ASSERT_EQ(v6->getOpcode(), Opcode::PHI);
  ASSERT_EQ(static_cast<PhiInstr *>(v6)->getOption(0).first, bbs[4]);
  ASSERT_EQ(static_cast<PhiInstr *>(v6)->getOption(0).second, v4);
  ASSERT_EQ(static_cast<PhiInstr *>(v6)->getOption(1).first, bbs[5]);
  ASSERT_EQ(static_cast<PhiInstr *>(v6)->getOption(1).second, v5);

  auto *v7 = v6->next();
  ASSERT_EQ(v7->getOpcode(), Opcode::SUB);
  ASSERT_EQ(v7->input(0), v6);
  ASSERT_EQ(v7->input(1), v0);

  auto *ret = v7->next();
  ASSERT_EQ(ret->getOpcode(), Opcode::RET);
  ASSERT_EQ(static_cast<RetInstr *>(ret)->getVal(), v7);

  // bb3
  auto *v8 = &*bbs[3]->begin();
  ASSERT_EQ(v8->getOpcode(), Opcode::EQ);
  ASSERT_EQ(v8->input(0), v2);
  ASSERT_EQ(v8->input(1), v0);

  auto *ifI = v8->next();
  ASSERT_EQ(ifI->getOpcode(), Opcode::IF);
}

TEST(Inline, Gcopy) {
  std::unique_ptr<Function> copy;
  {
    Function callee = createCalleeGraph();
    // std::cout << "Original:" << std::endl;
    // callee.dump(std::cout);
    copy = callee.copy();
  }

  //   std::cout << "copy:" << std::endl;
  //   copy->dump(std::cout);
}
