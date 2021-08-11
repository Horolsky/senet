#pragma once

#include "../src/s_ply.hpp"
#include "gtest/gtest.h"

TEST (eval, init)
{
    auto x = senet::xState();
    EXPECT_NO_THROW(senet::evaluate_state(x._seed));
}
