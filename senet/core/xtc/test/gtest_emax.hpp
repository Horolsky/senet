#pragma once

#include "../src/xtc.emax.hpp"
#include "gtest/gtest.h"




TEST (Emax, init)
{
    EXPECT_NO_THROW(xtc::Emax());
}

TEST (Emax, call)
{
    auto s = xtc::StrategyNode(xtc::State(),5);
    EXPECT_NO_THROW(xtc::Emax()(s));
}
