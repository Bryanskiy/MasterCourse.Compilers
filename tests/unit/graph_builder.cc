#include "function.hh"
#include "gtest/gtest.h"
#include <cstddef>
#include <cstdint>

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
// fn fact(v0: i32) -> i64 {
//     bb0: {
//         v1: i64 = const 1_64;
//         v2: i32 = const 2_32;
//         goto -> bb1;
//     }
//
//     bb1: {
//         v3: i32 = phi [v2, bb0], [b7, bb3];
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
    auto function = jade::Function{};
    auto v0 = function.appendParam(jade::Type::I32);

    auto bb0 = function.appendBB();
    auto bb1 = function.appendBB();
    auto bb2 = function.appendBB();
    auto bb3 = function.appendBB();

    {
        // check intrusive bb list
        // TODO: rewrite after iter impl
        ASSERT_EQ(bb0->prev(), nullptr);
        ASSERT_EQ(bb0->next(), bb1);
        ASSERT_EQ(bb1->prev(), bb0);
        ASSERT_EQ(bb1->next(), bb2);
        ASSERT_EQ(bb2->prev(), bb1);
        ASSERT_EQ(bb2->next(), bb3);
        ASSERT_EQ(bb3->prev(), bb2);
        ASSERT_EQ(bb3->next(), nullptr);
        ASSERT_NE(bb0->next(), bb3->prev());
    }

    auto builder0 = jade::InstrBulder{bb0};
    auto v1 = builder0.create<jade::CONST_I32>(1);
    auto v2 = builder0.create<jade::CONST_I64>(2);
    builder0.create<jade::GotoInstr>(bb1);

    {
        // check bb0
        ASSERT_EQ(v1->getType(), jade::Type::I32);
        ASSERT_EQ(v2->getType(), jade::Type::I64);
        ASSERT_EQ(v1->getValue(), 1);
        ASSERT_EQ(v2->getValue(), 2);
        ASSERT_EQ(v1->next(), v2);
        ASSERT_EQ(v2->prev(), v1);
    }

    auto builder1 = jade::InstrBulder{bb1};
    auto v3 = builder1.create<jade::PhiInstr>();
    // auto v4 = builder1.create<jade::BinaryInstr>(
    //     CAST(v3, jade::Value*),
    //     CAST(v0, jade::Value*),
    //     jade::BinaryInstr::Kind::LE
    // );
}
