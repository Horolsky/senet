#pragma once
#include "gtest_helper.hpp"

TEST (State, init)
{
    EXPECT_NO_THROW(xtc::State());
}

TEST (State, def_board)
{
    auto s = xtc::State();
    int board[30];
    s.board(board);
    int sample[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    bool eq = std::equal (std::begin(board), std::end (board), std::begin (sample));
    EXPECT_EQ(eq, true);
}

TEST (State, build_seed)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 5;
    int board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto s = xtc::State(agent, steps, board);
    EXPECT_EQ(s.seed(), 0b1010101010101010101010101010101010101010010001000100010001001011);
}

TEST (State, build_seed_corrupted)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 7;
    int board[30] {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    auto s = xtc::State(agent, steps, board);
    EXPECT_EQ(s.seed(), 0b1111111111111111111111111111111111111111111111111111111111111111);
}

TEST (State, board_get)
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

TEST (State, terminal)
{
    xtc::Unit agent = xtc::Unit::X;
    int steps = 1;
    int board[30] {0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto s = xtc::State(agent, steps, board);
    EXPECT_TRUE(s.is_terminal_node());
}


TEST (State, child)
{
    auto state = start_state(xtc::Unit::X, 1);
    xtc::ChanceNode new_state = state.child(0);
    int new_board[30] {1,0,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    for (int i = 0; i < 30; i++)
    {
        EXPECT_EQ(new_state.board(i), static_cast<xtc::Unit>(new_board[i]));
    }
}

TEST (State, assign)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, 5, board));

    EXPECT_EQ(state.agent(), xtc::Unit::X);
    EXPECT_EQ(state.steps(), 5);
    EXPECT_EQ(state.board(0), xtc::Unit::NONE);
    EXPECT_TRUE(state.is_terminal_node()); 

    state = start_state(xtc::Unit::Y, 1);
    
    EXPECT_EQ(state.agent(), xtc::Unit::Y);
    EXPECT_EQ(state.steps(), 1);
    EXPECT_EQ(state.board(0), xtc::Unit::X);
}

