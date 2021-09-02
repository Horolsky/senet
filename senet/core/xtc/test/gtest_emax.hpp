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

TEST (Emax, performance)
{
    int n_of_plays {1};
    int timer {1000};
    int depth1 {1};
    int depth2 {1};

    // uint64_t seed = xtc::StrategyNode(xtc::State(),1).seed();
    uint64_t seed = 11135337720068090533UL;
    EXPECT_NO_THROW(xtc::autoplay(seed, n_of_plays, depth1, depth2, timer));
}
