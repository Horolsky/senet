#pragma once

#include "../src/xtc.logic.hpp"
#include "gtest/gtest.h"



TEST (FuncStrategies, init)
{
    EXPECT_NO_THROW(xtc::FuncStrategies());
}
TEST (FuncIncrement, init)
{
    EXPECT_NO_THROW(xtc::FuncIncrement());
}

TEST (FuncStrategies, contains)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 1;
    int board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto state = xtc::State(agent, steps, board);
    xtc::Strategies s = xtc::FuncStrategies()(state);
    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}