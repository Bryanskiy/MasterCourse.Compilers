#include "function.hh"
#include "gtest/gtest.h"

TEST(GraphBuilder, Check) {
    ASSERT_TRUE(true);
}

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

    auto bb0 = function.append();
    auto bb1 = function.append();
    auto bb2 = function.append();
    auto bb3 = function.append();

    // check intrusive bb list
    {
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


}
