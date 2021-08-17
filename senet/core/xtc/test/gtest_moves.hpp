#pragma once

#include "../src/xtc.fwd.hpp"
#include "gtest/gtest.h"

TEST (xMoves, init)
{
    EXPECT_NO_THROW(xtc::Moves());
}