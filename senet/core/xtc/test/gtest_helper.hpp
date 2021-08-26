#pragma once
#include "../src/xtc.fwd.hpp"
#include "../src/xtc.dice.hpp"
#include "../src/xtc.state.hpp"
#include "../src/xtc.strategies.hpp"
#include "../src/xtc.eval.hpp"
#include "../src/xtc.emax.hpp"
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