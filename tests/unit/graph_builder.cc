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
}
