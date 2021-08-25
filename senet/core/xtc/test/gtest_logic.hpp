#pragma once

#include "../src/xtc.state.hpp"
#include "../src/xtc.strategies.hpp"
#include "../src/xtc.eval.hpp"
#include "gtest/gtest.h"

const int start_board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

xtc::StrategyNode start_state(xtc::Unit agent, int steps)
{
    auto state = xtc::StrategyNode(xtc::State(), agent, steps);
    return state;
}

void empty_board(int* board)
{   
    for (int i = 0; i < 30; i++)
    board[i] = 2;
}

TEST (Logic, start_strats)
{
    auto state = start_state(xtc::Unit::Y, 1);
    xtc::Strategies s = state.strategies();

    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}

TEST (Logic, simple_child)
{
    auto state = start_state(xtc::Unit::X, 1);
    xtc::ChanceNode new_state = state.child(0);
    int new_board[30] {1,0,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    for (int i = 0; i < 30; i++)
    {
        EXPECT_EQ(new_state.board(i), static_cast<xtc::Unit>(new_board[i]));
    }
}

TEST (Logic, assign)
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

TEST (Logic, MOVE)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    board[9] = 1;
    int chance = 5;
    auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, chance, board));
    
    int moves[6] {8,13,18,23,24,25};
    int steps[6] {5, 5, 5, 1, 1, 5};
    for (int i = 0; i < 6; i++)
    {
        state = state.child(moves[i]).child(steps[i]);
    }
    EXPECT_TRUE(state.is_terminal_node());   
    EXPECT_EQ(xtc::Eval::expectation(state), 0);   
}

TEST (Logic, ATTACK)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    board[9] = 1;
    int chance = 1;
    auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, chance, board));
    
    state = state.child(8).child(2);
    EXPECT_EQ(state.board(8), xtc::Unit::Y);
    EXPECT_EQ(state.board(9), xtc::Unit::X);
}


TEST (Logic, ATTACK_HOUSE)
{
    int board[30];
    empty_board(board);
    board[25] = 0;
    board[29] = 1;
    int chance = 4;
    auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, chance, board));
    
    EXPECT_EQ(state.strategies().actions(0), xtc::Action::ATTACK_HOUSE);
    
    state = state.child(25).child(3);
    EXPECT_EQ(state.board(29), xtc::Unit::X);
    EXPECT_EQ(state.board(26), xtc::Unit::Y); // DROWED    
}