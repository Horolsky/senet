#pragma once

#include "../src/xtc.internal.hpp"
#include "gtest/gtest.h"

TEST (State_eval, init)
{
    auto x = xtc::State();
    EXPECT_NO_THROW(x.evaluate());
}
