#pragma once

#include "../src/xtc.strategies.hpp"
#include "gtest/gtest.h"

TEST (Strategies, init)
{
    EXPECT_NO_THROW(xtc::Strategies());
}

TEST (Strategies, contains)
{
    xtc::Unit agent {xtc::Unit::Y};
    int mobility {5};
    int indici[5] {1,3,5,7,9};
    int actions[5] {0,0,0,0,0};
    auto s = xtc::Strategies(agent, mobility, indici, actions); 

    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(3));
    EXPECT_TRUE(s.contains(5));
    EXPECT_TRUE(s.contains(7));
    EXPECT_TRUE(s.contains(9));
}