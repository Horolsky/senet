#pragma once

#include "../src/xtc.dice.hpp"
#include "gtest/gtest.h"


TEST (Dice, init)
{
    EXPECT_NO_THROW(xtc::Dice().roll());
}