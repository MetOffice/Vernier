/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "vernier.h"

using ::testing::ExitedWithCode;

// Attempt to initialise Vernier without MPI being initialized.
TEST(TestMPINotInit, MpiNotInitialised) {

  // clang-format off
#ifdef USE_MPI
  [[maybe_unused]] int ierr;
  EXPECT_EXIT(
      { meto::vernier.init(); }, ExitedWithCode(EXIT_FAILURE),
      "MPIContext::init. MPI not initialized."),
      meto::vernier.finalize();
#else
  meto::vernier.init();
  meto::vernier.finalize();
#endif
  // clang-format on
}
