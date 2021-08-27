#pragma once
#include "gtest_helper.hpp"

TEST (Emax, init)
{
    EXPECT_NO_THROW(Emax());
}

TEST (Emax, call)
{
    auto s = StrategyNode(State(),1);
    EXPECT_NO_THROW(Emax()(s));
}

TEST (Emax, choice)
{
    auto s = StrategyNode(State(),1);
    int choice = Emax()(s);
    EXPECT_LE(choice, s.strategies().mobility());
}
