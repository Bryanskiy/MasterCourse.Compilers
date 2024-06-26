#include <cstddef>
#include <cstdint>

#include "IR.hh"
#include "function.hh"
#include "gtest/gtest.h"

using namespace jade;

TEST(GraphBuilder, Check) { ASSERT_TRUE(true); }

// C++ code:
//
// int64_t fact(int32_t n) {
//     int64_t res{1};
//     for(int32_t i{2}; i <= n; ++i) {
//         res *= i;
//     }

//     return res;
// }
//
// Jade IR:
//
//
// fn fact(v0: i32) -> i64 {
//     bb0: {
//         v1: i64 = const 1_64;
//         v2: i32 = const 2_32;
//         goto -> bb1;
//     }
//
//     bb1: {
//         v3: i32 = phi [v2, bb0], [v7, bb3];
//         v4: i1 = v3 <= v0;
//         if (v4, bb2, bb3);
//     }
//
//     bb2: { // false branch
//         v5: i64 = phi [v1, bb0], [v10, bb3];
//         ret v5;
//     }
//
//     bb3: { // true branch
//         v6: i32 = phi [v2, bb0], [v7, bb3]
//         v7: i32 = v6 + const 1_i32;
//         v8: i64 = v7 as i64;
//         v9: i64 = phi [v1, bb0], [v10, bb3];
//         v10: i64 = v8 * v9;
//         goto -> bb1;
//     }
// }
//
//      +-----+
//      | bb0 |
//      +-----+
//         |
//         V
//      +-----+    +-----+
//   +->| bb1 |--->| bb2 |
//   |  +-----+    +-----+
//   |     |
//   |     V
//   |  +-----+
//   ---| bb3 |
//      +-----+
TEST(GraphBuilder, Fib) {
  // TODO: macros for convinient graph generation

  auto function = Function{};
  auto bb0 = function.create<BasicBlock>();
  auto bb1 = function.create<BasicBlock>();
  auto bb2 = function.create<BasicBlock>();
  auto bb3 = function.create<BasicBlock>();

  {
    ASSERT_EQ(bb0->getId(), 0);
    ASSERT_EQ(bb1->getId(), 1);
    ASSERT_EQ(bb2->getId(), 2);
    ASSERT_EQ(bb3->getId(), 3);
  }

  {
    // check intrusive bb list
    // TODO: rewrite after iter impl
    ASSERT_EQ(bb0->getNext(), bb1);
    ASSERT_EQ(bb1->getPrev(), bb0);
    ASSERT_EQ(bb1->getNext(), bb2);
    ASSERT_EQ(bb2->getPrev(), bb1);
    ASSERT_EQ(bb2->getNext(), bb3);
    ASSERT_EQ(bb3->getPrev(), bb2);
    ASSERT_NE(bb0->getNext(), bb3->getPrev());
  }

  // bb0
  auto v0 = bb0->create<ParamInstr>(Type::create<Type::I32>());
  auto v1 = bb0->create<ConstI64>(1);
  auto v2 = bb0->create<ConstI32>(2);
  bb0->create<GotoInstr>(bb1);

  {
    ASSERT_EQ(v1->getType(), Type::I64);
    ASSERT_EQ(v2->getType(), Type::I32);
    ASSERT_EQ(v1->getValue(), 1);
    ASSERT_EQ(v2->getValue(), 2);
    ASSERT_EQ(v1->getNext(), v2);
    ASSERT_EQ(v2->getPrev(), v1);

    ASSERT_EQ(v1->getParent(), bb0);
  }

  // bb1
  auto v3 = bb1->create<PhiInstr>(Type::create<Type::I32>());
  auto v4 = bb1->create<CmpInstr>(v3, v0, Opcode::LE);
  auto if_ = bb1->create<IfInstr>(v4, bb2, bb3);

  {
    ASSERT_EQ(v3->getPrev(), nullptr);
    ASSERT_EQ(v3->getNext(), v4);
    ASSERT_EQ(v4->getPrev(), v3);
    ASSERT_EQ(v4->getNext(), if_);
    ASSERT_EQ(if_->getPrev(), v4);

    ASSERT_EQ(v3->getType(), Type::I32);
    ASSERT_EQ(v4->getType(), Type::I1);

    ASSERT_EQ(v3->getParent(), bb1);
  }

  // bb2
  auto v5 = bb2->create<PhiInstr>(Type::create<Type::I64>());
  auto ret = bb2->create<RetInstr>(v5);

  {
    ASSERT_EQ(v5->getNext(), ret);
    ASSERT_EQ(ret->getPrev(), v5);

    ASSERT_EQ(v5->getType(), Type::I64);
  }

  // bb3
  auto v6 = bb3->create<PhiInstr>(Type::create<Type::I32>());
  auto c = bb3->create<ConstI32>(1);
  auto v7 = bb3->create<BinaryOp>(v6, c, Opcode::ADD);
  auto v8 = bb3->create<CastInstr>(v7, Type::I64);
  auto v9 = bb3->create<PhiInstr>(Type::create<Type::I64>());
  auto v10 = bb3->create<BinaryOp>(v8, v9, Opcode::MUL);
  bb3->create<GotoInstr>(bb1);

  {
    ASSERT_EQ(v6->getType(), Type::I32);
    ASSERT_EQ(c->getType(), Type::I32);
    ASSERT_EQ(c->getValue(), 1);
    ASSERT_EQ(v6->getType(), Type::I32);
    ASSERT_EQ(v8->getType(), Type::I64);
    ASSERT_EQ(v9->getType(), Type::I64);
    ASSERT_EQ(v10->getType(), Type::I64);
  }

  // PHI
  // addOption checks types
  v3->addOption(v2, bb0);
  v3->addOption(v7, bb3);

  v5->addOption(v1, bb0);
  v5->addOption(v10, bb3);

  v6->addOption(v2, bb0);
  v6->addOption(v7, bb3);

  v9->addOption(v1, bb0);
  v9->addOption(v10, bb3);

  {
    // check successors and preds.
    auto checker = [](auto lhsBegin, auto lhsEnd, auto rhsBegin, auto rhsEnd) {
      for (; lhsBegin != lhsEnd; ++lhsBegin, ++rhsBegin) {
        ASSERT_EQ(*lhsBegin, *rhsBegin);
      }

      ASSERT_EQ(rhsBegin, rhsEnd);
    };

    {
      auto lhsPreds = std::array<BasicBlock *, 0>{};
      auto rhsPreds = bb0->predecessors();
      checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(),
              lhsPreds.end());

      auto lhsSuccs = std::array<BasicBlock *, 1>{bb1};
      auto rhsSuccs = bb0->successors();
      checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(),
              lhsSuccs.end());
    }

    {
      auto lhsPreds = std::array<BasicBlock *, 2>{bb0, bb3};
      auto rhsPreds = bb1->predecessors();
      checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(),
              lhsPreds.end());

      auto lhsSuccs = std::array<BasicBlock *, 2>{bb2, bb3};
      auto rhsSuccs = bb1->successors();
      checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(),
              lhsSuccs.end());
    }

    {
      auto lhsPreds = std::array<BasicBlock *, 1>{bb1};
      auto rhsPreds = bb3->predecessors();
      checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(),
              lhsPreds.end());

      auto lhsSuccs = std::array<BasicBlock *, 1>{bb1};
      auto rhsSuccs = bb3->successors();
      checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(),
              lhsSuccs.end());
    }

    {
      auto lhsPreds = std::array<BasicBlock *, 1>{bb1};
      auto rhsPreds = bb2->predecessors();
      checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(),
              lhsPreds.end());

      auto lhsSuccs = std::array<BasicBlock *, 0>{};
      auto rhsSuccs = bb2->successors();
      checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(),
              lhsSuccs.end());
    }
  }
}
