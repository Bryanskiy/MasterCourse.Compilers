#include <array>

#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "linearOrder.hh"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace jade;

TEST(LinearOrder, Check) { ASSERT_TRUE(true); }

bool checkOrder(const std::vector<BasicBlock *> lhs,
                std::vector<BasicBlock *> rhs) {
  return lhs == rhs;
}

TEST(LinearOrder, Example1) {
  auto function = example1();
  auto graph = function.getBasicBlocks();

  auto range = graph.nodes();
  std::vector<BasicBlock *> bbs;
  for (auto it = range.begin(); it != range.end(); ++it) {
    bbs.push_back(&*it);
  }

  auto actual = LinearOrder(graph).linearize();
  ASSERT_TRUE(checkOrder(actual, {bbs[0], bbs[1], bbs[3], bbs[2]}));
}

TEST(LinearOrder, Example2) {
  auto function = example2();
  auto graph = function.getBasicBlocks();

  auto range = graph.nodes();
  std::vector<BasicBlock *> bbs;
  for (auto it = range.begin(); it != range.end(); ++it) {
    bbs.push_back(&*it);
  }

  auto actual = LinearOrder(graph).linearize();
  ASSERT_TRUE(checkOrder(
      actual, {bbs[0], bbs[1], bbs[4], bbs[6], bbs[3], bbs[2], bbs[5]}));
}

TEST(LinearOrder, Example3) {
  auto function = example3();
  auto graph = function.getBasicBlocks();

  auto range = graph.nodes();
  std::vector<BasicBlock *> bbs;
  for (auto it = range.begin(); it != range.end(); ++it) {
    bbs.push_back(&*it);
  }

  auto actual = LinearOrder(graph).linearize();
  ASSERT_TRUE(
      checkOrder(actual, {bbs[0], bbs[1], bbs[10], bbs[2], bbs[3], bbs[4],
                          bbs[5], bbs[6], bbs[9], bbs[7], bbs[8]}));
}

TEST(LinearOrder, Example4) {
  auto function = example4();
  auto graph = function.getBasicBlocks();

  auto range = graph.nodes();
  std::vector<BasicBlock *> bbs;
  for (auto it = range.begin(); it != range.end(); ++it) {
    bbs.push_back(&*it);
  }

  auto actual = LinearOrder(graph).linearize();
  ASSERT_TRUE(checkOrder(actual, {bbs[0], bbs[1], bbs[8], bbs[7], bbs[6],
                                  bbs[2], bbs[3], bbs[4], bbs[5]}));
}
