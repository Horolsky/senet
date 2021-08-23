#pragma once

#include "../src/xtc.eval.hpp"
#include "gtest/gtest.h"

xtc::State get_start_state()
{
    auto state = xtc::State();
    state.set_steps(1);
    return state;
}

TEST (State_expect, init)
{
    EXPECT_NO_THROW(xtc::Eval::expectation(xtc::State()));
}

TEST (State_expect, call_basic)
{
    auto state = get_start_state();
    EXPECT_NO_THROW(xtc::Eval()(state));
}

TEST (State_expect, call_lin)
{
    auto state = get_start_state();
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval::Func::LINEAR)(state));
}

TEST (State_expect, call_poly)
{
    auto state = get_start_state();
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval::Func::POLYNOMIAL)(state));
}