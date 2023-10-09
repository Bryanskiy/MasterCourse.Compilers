#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "function.hh"

using namespace jade;

TEST(GraphBuilder, Check) {
    ASSERT_TRUE(true);
}

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
    auto v0 = function.appendParam(Type::I32);

    auto bb0 = function.appendBB();
    auto bb1 = function.appendBB();
    auto bb2 = function.appendBB();
    auto bb3 = function.appendBB();

    bb0->addSuccessor(bb1);

    bb1->addSuccessor(bb2);
    bb1->addSuccessor(bb3);
    bb1->addPredecessor(bb0);
    bb1->addPredecessor(bb3);

    bb2->addPredecessor(bb1);

    bb3->addPredecessor(bb1);
    bb3->addSuccessor(bb1);

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
    auto builder0 = InstrBulder{bb0};
    auto v1 = builder0.create<ConstI64>(1);
    auto v2 = builder0.create<ConstI32>(2);
    builder0.create<GotoInstr>(bb1);

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
    auto builder1 = InstrBulder{bb1};
    auto v3 = builder1.create<PhiInstr>(Type::create<Type::I32>());
    auto v4 = builder1.create<CmpInstr>(
        static_cast<Value*>(v3),
        static_cast<Value*>(v0),
        CmpInstr::Kind::LE
    );
    auto if_ = builder1.create<IfInstr>(
        static_cast<Value*>(v4),
        bb2,
        bb3
    );

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
    auto builder2 = InstrBulder{bb2};
    auto v5 = builder2.create<PhiInstr>(Type::create<Type::I64>());
    auto ret = builder2.create<RetInstr>(v5);

    {
        ASSERT_EQ(v5->getNext(), ret);
        ASSERT_EQ(ret->getPrev(), v5);

        ASSERT_EQ(v5->getType(), Type::I64);
    }

    // bb3
    auto builder3 = InstrBulder{bb3};
    auto v6 = builder3.create<PhiInstr>(Type::create<Type::I32>());
    auto c = builder3.create<ConstI32>(1);
    auto v7 = builder3.create<BinaryOp>(
        static_cast<Value*>(v6),
        static_cast<Value*>(c),
        BinaryOp::Kind::ADD
    );
    auto v8 = builder3.create<CastInstr>(v7, Type::I64);
    auto v9 = builder3.create<PhiInstr>(Type::create<Type::I64>());
    auto v10 = builder3.create<BinaryOp>(
        static_cast<Value*>(v8),
        static_cast<Value*>(v9),
        BinaryOp::Kind::MUL
    );
    builder3.create<GotoInstr>(bb1);

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
            for(; lhsBegin != lhsEnd; ++lhsBegin, ++rhsBegin) {
                ASSERT_EQ(*lhsBegin, *rhsBegin);
            }

            ASSERT_EQ(rhsBegin, rhsEnd);
        };

        {
            auto lhsPreds = std::array<BasicBlock*, 0>{};
            auto rhsPreds = bb0->predecessors();
            checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(), lhsPreds.end());

            auto lhsSuccs = std::array<BasicBlock*, 1>{bb1};
            auto rhsSuccs = bb0->successors();
            checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(), lhsSuccs.end());
        }

        {
            auto lhsPreds = std::array<BasicBlock*, 2>{bb0, bb3};
            auto rhsPreds = bb1->predecessors();
            checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(), lhsPreds.end());

            auto lhsSuccs = std::array<BasicBlock*, 2>{bb2, bb3};
            auto rhsSuccs = bb1->successors();
            checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(), lhsSuccs.end());
        }

        {
            auto lhsPreds = std::array<BasicBlock*, 1>{bb1};
            auto rhsPreds = bb3->predecessors();
            checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(), lhsPreds.end());

            auto lhsSuccs = std::array<BasicBlock*, 1>{bb1};
            auto rhsSuccs = bb3->successors();
            checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(), lhsSuccs.end());
        }

        {
            auto lhsPreds = std::array<BasicBlock*, 1>{bb1};
            auto rhsPreds = bb2->predecessors();
            checker(rhsPreds.begin(), rhsPreds.end(), lhsPreds.begin(), lhsPreds.end());

            auto lhsSuccs = std::array<BasicBlock*, 0>{};
            auto rhsSuccs = bb2->successors();
            checker(rhsSuccs.begin(), rhsSuccs.end(), lhsSuccs.begin(), lhsSuccs.end());
        }
    }
}
