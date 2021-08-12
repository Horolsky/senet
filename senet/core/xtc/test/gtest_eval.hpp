#pragma once

#include "../src/f_eval.hpp"
#include "gtest/gtest.h"

TEST (eval, init)
{
    auto x = xtc::xState();
    EXPECT_NO_THROW(xtc::evaluate_state(x));
}
