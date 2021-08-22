#pragma once

#include "../src/xtc.emax.hpp"
#include "gtest/gtest.h"




TEST (Emax, init)
{
    EXPECT_NO_THROW(xtc::Emax());
}

TEST (Emax, call)
{
    xtc::Unit agent = xtc::Unit::Y;
    int steps = 5;
    int board[30] {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    auto s = xtc::State(agent, steps, board);

    EXPECT_NO_THROW(xtc::Emax()(s));
}
