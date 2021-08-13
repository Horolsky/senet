#pragma once

#include "../src/s_state.hpp"
#include "gtest/gtest.h"

TEST (xState, init)
{
    EXPECT_NO_THROW(xtc::xState());
}

TEST (xState, def_seed)
{
    auto s = xtc::xState();
    auto board = s.board();
    std::vector<uint8_t> sample {0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    bool eq = std::equal (board.begin (), board.end (), sample.begin ());
    EXPECT_EQ(eq, true);
}
