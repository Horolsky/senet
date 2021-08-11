#pragma once

#include "../src/s_ply.hpp"
#include "gtest/gtest.h"

TEST (xPly, init)
{
    auto x = senet::xPly();
    EXPECT_EQ(static_cast<uint8_t>(x.agent()), 0);
}
