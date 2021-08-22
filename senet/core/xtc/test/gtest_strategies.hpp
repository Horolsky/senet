#pragma once

#include "../src/xtc.objects.hpp"
#include "gtest/gtest.h"

TEST (Strategies, init)
{
    EXPECT_NO_THROW(xtc::Strategies());
}