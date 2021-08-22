#pragma once

#include "../src/xtc.objects.hpp"
#include "gtest/gtest.h"

TEST (xState, init)
{
    EXPECT_NO_THROW(xtc::State());
}

TEST (xState, def_board)
{
    auto s = xtc::State();
    int board[30];
    s.board(board);
    int sample[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    bool eq = std::equal (std::begin(board), std::end (board), std::begin (sample));
    EXPECT_EQ(eq, true);
}

TEST (xState, build_seed)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 5;
    int board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto s = xtc::State(agent, steps, board);
    EXPECT_EQ(s.seed(), 0b1010101010101010101010101010101010101010010001000100010001001011);
}

TEST (xState, build_seed_corrupted)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 7;
    int board[30] {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    auto s = xtc::State(agent, steps, board);
    EXPECT_EQ(s.seed(), 0b1111111111111111111111111111111111111111111111111111111111111111);
}

TEST (xState, board_get)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 1;
    int board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto s = xtc::State(agent, steps, board);
    for (int i = 0; i < 30; i++)
    {
        xtc::Unit expected = static_cast<xtc::Unit>(board[i]);
        EXPECT_EQ(s.board(i), expected);
    }
    
}