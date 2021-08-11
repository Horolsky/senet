#pragma once

#include "../src/s_event.hpp"
#include "gtest/gtest.h"

TEST (xEvent, init)
{
    EXPECT_NO_THROW(senet::xEvent());
}
