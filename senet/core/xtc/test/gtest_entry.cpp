#include "gtest/gtest.h"

#include "gtest_dice.hpp"
#include "gtest_state.hpp"
#include "gtest_strategies.hpp"
#include "gtest_logic.hpp"
#include "gtest_eval.hpp"
#include "gtest_emax.hpp"

int
main (int argc, char **argv)
{
  printf ("Running %s...\n", argv[0]);

  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}