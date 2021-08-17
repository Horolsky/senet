#pragma once

#include "../src/xtc.fwd.hpp"
#include "gtest/gtest.h"

TEST (Event, init)
{
    EXPECT_NO_THROW(xtc::Event());
}
