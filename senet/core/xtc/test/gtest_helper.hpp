#pragma once
#include "../src/xtc.fwd.hpp"
#include "../src/xtc.dice.hpp"
#include "../src/xtc.state.hpp"
#include "../src/xtc.strategies.hpp"
#include "../src/xtc.eval.hpp"
#include "../src/xtc.emax.hpp"
#include "../src/xtc.auto.hpp"
#include "gtest/gtest.h"

using namespace xtc;

const int start_board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

StrategyNode start_state(Unit agent, int steps)
{
    auto state = StrategyNode(State(), agent, steps);
    return state;
}

void empty_board(int* board)
{   
    for (int i = 0; i < 30; i++)
    board[i] = 2;
}

bool check_board(int* board, const State& state)
{
    for (int i = 0; i < 30; i++)
    if (board[i] != static_cast<int>(state.board(i))) return false;
    return true;
}