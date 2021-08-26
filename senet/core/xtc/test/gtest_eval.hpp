#pragma once
#include "gtest_helper.hpp"

TEST (Eval, init)
{
    EXPECT_NO_THROW(xtc::Eval::expectation(xtc::State()));
}

TEST (Eval, call_basic)
{
    auto state = start_state(xtc::Unit::X,1);
    EXPECT_NO_THROW(xtc::Eval()(state));
}

TEST (Eval, call_lin)
{
    auto state = start_state(xtc::Unit::X,1);
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval::FuncID::LINEAR)(state));
}

TEST (Eval, call_poly)
{
    auto state = start_state(xtc::Unit::X,1);
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval::FuncID::POLYNOMIAL)(state));
}

TEST (Eval, constructors)
{
    auto t = xtc::Eval(xtc::Eval::FuncID::POLYNOMIAL);
    EXPECT_NO_THROW(xtc::Eval(t));
    EXPECT_NO_THROW(xtc::Eval(xtc::Eval(xtc::Eval::FuncID::POLYNOMIAL)));
}

TEST (Eval, def_coefs)
{
    auto t = xtc::Eval(xtc::Eval::FuncID::POLYNOMIAL);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::stepsum), 1);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::mobility), 0);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::priority), 0);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::attack), 0);
}

TEST (Eval, custom_coefs)
{
    auto t = xtc::Eval(xtc::Eval::FuncID::POLYNOMIAL, {1,1,2,4});
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::stepsum), .125);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::mobility), .125);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::priority), .25);
    EXPECT_EQ(t.coef(xtc::Eval::CoefID::attack), .5);
}