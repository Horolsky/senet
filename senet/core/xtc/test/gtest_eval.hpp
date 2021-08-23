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

TEST (State_expect, constructors)
{
    auto t = xtc::Eval(xtc::Eval::Func::POLYNOMIAL);
    EXPECT_NO_THROW(xtc::Eval(t));
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval(xtc::Eval::Func::POLYNOMIAL)));
}

TEST (State_expect, def_coefs)
{
    auto t = xtc::Eval(xtc::Eval::Func::POLYNOMIAL);
    EXPECT_EQ(t.coef(0), 1);
    EXPECT_EQ(t.coef(1), 0);
    EXPECT_EQ(t.coef(2), 0);
    EXPECT_EQ(t.coef(3), 0);
}

TEST (State_expect, custom_coefs)
{
    auto t = xtc::Eval(xtc::Eval::Func::POLYNOMIAL, {1,1,2,4});
    EXPECT_EQ(t.coef(0), .125);
    EXPECT_EQ(t.coef(1), .125);
    EXPECT_EQ(t.coef(2), .25);
    EXPECT_EQ(t.coef(3), .5);
}