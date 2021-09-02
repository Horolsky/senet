#pragma once
#include "gtest_helper.hpp"

TEST (Logic, SKIP)
{
    int board[30];
    empty_board(board);
    board[0] = 0;
    board[1] = 1;
    board[2] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 1);
    EXPECT_EQ(strats.indici(0), House::SKIPTURN);
    EXPECT_EQ(strats.actions(0), Action::SKIP);

    state = state.choice(House::SKIPTURN).chance(1);
    EXPECT_EQ(state.agent(), Unit::X);//repeat
    EXPECT_TRUE(check_board(board, state));
}

TEST (Logic, MOVE)
{
    int board[30];
    empty_board(board);
    board[0] = 0;
    board[6] = 1;
    int chance = 3;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 1);
    EXPECT_EQ(strats.indici(0), 0);
    EXPECT_EQ(strats.actions(0), Action::MOVE);   

    state = state.choice(0).chance(1);
    EXPECT_EQ(state.agent(), Unit::Y);
    board[0] = 2;
    board[3] = 0;
    EXPECT_TRUE(check_board(board, state));
}

TEST (Logic, RETREAT)
{
    int board[30];
    empty_board(board);
    board[5] = 0;
    board[6] = 1;
    board[7] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 1);
    EXPECT_EQ(strats.indici(0), 5);
    EXPECT_EQ(strats.actions(0), Action::RETREAT);   

    state = state.choice(5).chance(1);
    EXPECT_EQ(state.agent(), Unit::X);
    board[5] = 2;
    board[4] = 0;
    EXPECT_TRUE(check_board(board, state));
}

TEST (Logic, SWAPBACK)
{
    int board[30];
    empty_board(board);
    board[4] = 1;
    board[5] = 0;
    board[6] = 1;
    board[7] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 1);
    EXPECT_EQ(strats.indici(0), 5);
    EXPECT_EQ(strats.actions(0), Action::SWAPBACK);   

    state = state.choice(5).chance(1);
    EXPECT_EQ(state.agent(), Unit::X);
    board[5] = 1;
    board[4] = 0;
    EXPECT_TRUE(check_board(board, state));
}

TEST (Logic, DROW)
{
    int board[30];
    empty_board(board);
    board[House::WATERS] = 0;
    board[0] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 2);
    EXPECT_EQ(strats.indici(0), House::WATERS);
    EXPECT_EQ(strats.actions(0), Action::DROW);
    EXPECT_EQ(strats.indici(1), House::SKIPTURN);
    EXPECT_EQ(strats.actions(1), Action::SKIP);   
}

TEST (Logic, ATTACK)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    board[9] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    state = state.choice(8).chance(2);
    EXPECT_EQ(state.board(8), Unit::Y);
    EXPECT_EQ(state.board(9), Unit::X);
}

TEST (Logic, ATTACK_HOUSE)
{
    int board[30];
    empty_board(board);
    board[25] = 0;
    board[29] = 1;
    int chance = 4;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    EXPECT_EQ(state.strategies().actions(0), Action::ATTACK_HOUSE);
    
    state = state.choice(25).chance(3);
    EXPECT_EQ(state.board(29), Unit::X);
    EXPECT_EQ(state.board(26), Unit::Y); // DROWED    
}

TEST (Logic, ESCAPE)
{
    int board[30];
    empty_board(board);
    board[House::WATERS] = 0;
    board[0] = 1;
    int chance = 4;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 1);
    EXPECT_EQ(strats.indici(0), House::WATERS);
    EXPECT_EQ(strats.actions(0), Action::ESCAPE);
}

TEST (Logic, PANIC)
{
    int board[30];
    empty_board(board);
    board[House::TRUTHS] = 0;
    board[0] = 1;
    board[House::BEAUTY] = 1;
    int chance = 1;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    auto strats = state.strategies();
    EXPECT_EQ(strats.mobility(), 2);
    EXPECT_EQ(strats.indici(0), House::TRUTHS);
    EXPECT_EQ(strats.actions(0), Action::RETREAT);
    EXPECT_EQ(strats.indici(1), House::TRUTHS);
    EXPECT_EQ(strats.actions(1), Action::PANIC);
}

TEST (Logic, _complex_1)
{
    int board[30];
    empty_board(board);
    board[8] = 0;
    board[9] = 1;
    int chance = 5;
    auto state = StrategyNode(State::build_seed(Unit::X, chance, board));
    
    int moves[6] {8,13,18,23,24,25};
    int steps[6] {5, 5, 1, 1, 5, 5};
    for (int i = 0; i < 6; i++)
    {
        auto strat = state.choice(moves[i]);
        strat.board(board);
        state = strat.chance(steps[i]);
        
    }
    EXPECT_TRUE(state.is_terminal_node());   
    EXPECT_EQ(Eval::expectation(state), 0);   
}

TEST(Logic, REBIRTH_OCCUPIED)
{
    uint64_t seed = 12143157482611190434;
    auto node = StrategyNode(seed);
    auto strats = node.strategies();
    EXPECT_GT(strats.mobility(), 0);
}