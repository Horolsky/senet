#pragma once
#include "gtest_helper.hpp"

TEST (Strategies, init)
{
    EXPECT_NO_THROW(xtc::Strategies());
}

TEST (Strategies, seed_contains)
{
    xtc::Unit agent {xtc::Unit::Y};
    int mobility {5};
    int indici[5] {1,3,5,7,9};
    int actions[5] {0,0,0,0,0};
    auto s = xtc::Strategies(agent, mobility, indici, actions); 

    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}

TEST (Strategies, y_starts)
{
    auto state = start_state(xtc::Unit::Y, 1);
    xtc::Strategies s = state.strategies();

    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}