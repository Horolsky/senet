#pragma once

#include "../src/s_moves.hpp"
#include "gtest/gtest.h"

TEST (xMoves, init)
{
    EXPECT_NO_THROW(xtc::xMoves());
}