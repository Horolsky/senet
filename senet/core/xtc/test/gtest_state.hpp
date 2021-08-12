#pragma once

#include "../src/s_state.hpp"
#include "gtest/gtest.h"

TEST (xState, init)
{
    EXPECT_NO_THROW(xtc::xState());
}
