#pragma once
#include "gtest_helper.hpp"

TEST (Dice, init)
{
    EXPECT_NO_THROW(Dice().roll());
}

TEST (Dice, P)
{
    double omega {0};
    for (double p : Dice::P)
    {
        omega+=p;
    }
    EXPECT_NEAR(omega,1,.00001);
}