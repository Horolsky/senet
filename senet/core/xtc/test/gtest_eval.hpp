#pragma once

#include "../src/xtc.eval.hpp"
#include "gtest/gtest.h"



TEST (State_expect, init)
{
    EXPECT_NO_THROW(xtc::Eval::expectation(xtc::State()));
}
