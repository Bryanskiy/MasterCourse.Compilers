#include "graphs.hh"
#include "IR.hh"
#include "function.hh"
#include <array>

using namespace jade;

Function example1() {
  auto function = Function{};

  std::array<BasicBlock *, 4> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }
  // TODO: more instrs
  auto v0 = function.create<Param>(Type::create<Type::I32>());
  {
    auto builder = InstrBulder(bbs[0]);
    builder.create<GotoInstr>(bbs[1]);
    bbs[0]->addSuccessor(bbs[1]);
  }
  {
    auto builder = InstrBulder(bbs[1]);
    builder.create<IfInstr>(v0, bbs[2], bbs[3]);
    bbs[1]->addSuccessor(bbs[2]);
    bbs[1]->addSuccessor(bbs[3]);
  }
  {
    auto builder = InstrBulder(bbs[3]);
    builder.create<GotoInstr>(bbs[1]);
    bbs[3]->addSuccessor(bbs[1]);
  }
  {
    auto builder = InstrBulder(bbs[2]);
    builder.create<RetInstr>();
  }

  return function;
}

Function example2() {
  auto function = Function{};

  std::array<BasicBlock *, 7> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }
  // TODO: more instrs
  auto v0 = function.create<Param>(Type::create<Type::I32>());
  {
    auto builder = InstrBulder(bbs[0]);
    builder.create<GotoInstr>(bbs[1]);
    bbs[0]->addSuccessor(bbs[1]);
  }

  {
    auto builder = InstrBulder(bbs[1]);
    builder.create<IfInstr>(v0, bbs[2], bbs[4]);
    bbs[1]->addSuccessor(bbs[2]);
    bbs[1]->addSuccessor(bbs[4]);
  }
  {
    auto builder = InstrBulder(bbs[2]);
    builder.create<GotoInstr>(bbs[5]);
    bbs[2]->addSuccessor(bbs[5]);
  }
  {
    auto builder = InstrBulder(bbs[3]);
    builder.create<GotoInstr>(bbs[5]);
    bbs[3]->addSuccessor(bbs[5]);
  }
  {
    auto builder = InstrBulder(bbs[4]);
    builder.create<IfInstr>(v0, bbs[3], bbs[6]);
    bbs[4]->addSuccessor(bbs[3]);
    bbs[4]->addSuccessor(bbs[6]);
  }

  {
    auto builder = InstrBulder(bbs[6]);
    builder.create<GotoInstr>(bbs[5]);
    bbs[6]->addSuccessor(bbs[5]);
  }
  {
    auto builder = InstrBulder(bbs[5]);
    builder.create<RetInstr>();
  }

  return function;
}

Function example3() {
  auto function = Function{};

  std::array<BasicBlock *, 11> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  bbs[0]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[2]);
  bbs[2]->addSuccessor(bbs[3]);
  bbs[3]->addSuccessor(bbs[4]);
  bbs[4]->addSuccessor(bbs[5]);
  bbs[5]->addSuccessor(bbs[6]);
  bbs[6]->addSuccessor(bbs[7]);
  bbs[7]->addSuccessor(bbs[8]);
  bbs[6]->addSuccessor(bbs[9]);
  bbs[5]->addSuccessor(bbs[4]);
  bbs[9]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[10]);
  bbs[10]->addSuccessor(bbs[2]);
  bbs[3]->addSuccessor(bbs[2]);

  return function;
}

Function example4() {
  auto function = Function{};

  std::array<BasicBlock *, 9> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  bbs[0]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[2]);
  bbs[2]->addSuccessor(bbs[3]);
  bbs[3]->addSuccessor(bbs[4]);
  bbs[4]->addSuccessor(bbs[5]);
  bbs[4]->addSuccessor(bbs[2]);
  bbs[1]->addSuccessor(bbs[8]);
  bbs[8]->addSuccessor(bbs[7]);
  bbs[7]->addSuccessor(bbs[6]);
  bbs[6]->addSuccessor(bbs[5]);
  bbs[6]->addSuccessor(bbs[4]);
  bbs[8]->addSuccessor(bbs[3]);
  bbs[7]->addSuccessor(bbs[1]);

  return function;
}

Function example5() {
  auto function = Function{};

  std::array<BasicBlock *, 11> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  bbs[0]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[2]);
  bbs[1]->addSuccessor(bbs[4]);
  bbs[4]->addSuccessor(bbs[3]);
  bbs[3]->addSuccessor(bbs[1]);

  return function;
}

Function example6() {
  auto function = Function{};

  std::array<BasicBlock *, 11> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  bbs[0]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[2]);
  bbs[2]->addSuccessor(bbs[3]);
  bbs[3]->addSuccessor(bbs[4]);
  bbs[4]->addSuccessor(bbs[1]);
  bbs[2]->addSuccessor(bbs[5]);
  bbs[3]->addSuccessor(bbs[5]);

  return function;
}

Function example7() {
  auto function = Function{};

  std::array<BasicBlock *, 11> bbs;
  for (std::size_t i = 0; i < bbs.size(); ++i) {
    bbs[i] = function.create<BasicBlock>();
  }

  bbs[0]->addSuccessor(bbs[1]);
  bbs[1]->addSuccessor(bbs[2]);
  bbs[1]->addSuccessor(bbs[4]);
  bbs[2]->addSuccessor(bbs[3]);
  bbs[2]->addSuccessor(bbs[5]);
  bbs[4]->addSuccessor(bbs[5]);
  bbs[5]->addSuccessor(bbs[6]);
  bbs[6]->addSuccessor(bbs[7]);
  bbs[6]->addSuccessor(bbs[1]);
  bbs[7]->addSuccessor(bbs[0]);

  return function;
}
