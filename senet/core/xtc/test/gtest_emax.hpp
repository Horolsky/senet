#pragma once
#include "gtest_helper.hpp"

TEST (Emax, init)
{
    EXPECT_NO_THROW(Emax());
}

TEST (Emax, call)
{
    auto s = StrategyNode(State(),5);
    EXPECT_NO_THROW(Emax()(s));
}
