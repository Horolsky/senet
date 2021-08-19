#pragma once

#include "../src/xtc.internal.hpp"
#include "gtest/gtest.h"

TEST (xMoves, init)
{
    EXPECT_NO_THROW(xtc::Moves());
}