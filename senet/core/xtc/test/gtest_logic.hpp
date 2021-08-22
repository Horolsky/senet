#pragma once

#include "../src/xtc.logic.hpp"
#include "gtest/gtest.h"

xtc::State get_start_state(xtc::Unit agent, int steps)
{
    auto state = xtc::State();
    state.set_agent(agent);
    state.set_steps(steps);
    return state;
}
const int start_board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

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
    auto state = get_start_state(xtc::Unit::Y, 1);
    xtc::Strategies s = xtc::FuncStrategies()(state);
    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}

TEST (FuncIncrement, increment_1)
{
    auto state = get_start_state(xtc::Unit::X, 1);
    xtc::State new_state = xtc::FuncIncrement()(state,0);
    int new_board[30] {1,0,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    for (int i = 0; i < 30; i++)
    {
        EXPECT_EQ(new_state.board(i), static_cast<xtc::Unit>(new_board[i]));
    }
}
TEST (FuncIncrement, increment_chance_node)
{
    
    auto test = [](){
        xtc::FuncIncrement incr {};
        incr(xtc::State(),0);
    };
    EXPECT_THROW(test(), std::logic_error);
}