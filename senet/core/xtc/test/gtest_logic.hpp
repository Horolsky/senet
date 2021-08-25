#pragma once

#include "../src/xtc.state.hpp"
#include "../src/xtc.strategies.hpp"
#include "gtest/gtest.h"

xtc::StrategyNode get_start_state(xtc::Unit agent, int steps)
{
    auto state = xtc::StrategyNode(xtc::State(), agent, steps);
    return state;
}
const int start_board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};


TEST (Strategies, contains)
{
    auto state = get_start_state(xtc::Unit::Y, 1);
    xtc::Strategies s = state.strategies();

    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}

TEST (StrategyNode, child)
{
    auto state = get_start_state(xtc::Unit::X, 1);
    xtc::ChanceNode new_state = state.child(0);
    int new_board[30] {1,0,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    for (int i = 0; i < 30; i++)
    {
        EXPECT_EQ(new_state.board(i), static_cast<xtc::Unit>(new_board[i]));
    }
}
TEST (ChanceNode, child)
{
    EXPECT_EQ(xtc::ChanceNode().steps(), 0);
}