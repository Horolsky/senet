#include "gtest/gtest.h"


#include "gtest_state.hpp"
#include "gtest_moves.hpp"
#include "gtest_event.hpp"
#include "gtest_eval.hpp"

int
main (int argc, char **argv)
{
  printf ("Running %s...\n", argv[0]);

  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}