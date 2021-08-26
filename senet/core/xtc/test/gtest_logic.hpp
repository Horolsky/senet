#pragma once
#include "gtest_helper.hpp"

TEST (Logic, MOVE)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    board[9] = 1;
    int chance = 5;
    auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, chance, board));
    
    int moves[6] {8,13,18,23,24,25};
    int steps[6] {5, 5, 1, 1, 5, 5};
    for (int i = 0; i < 6; i++)
    {
        auto strat = state.child(moves[i]);
        strat.board(board);
        state = strat.child(steps[i]);
        
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