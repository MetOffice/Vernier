/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vernier.h"

// Attempt to initialise Vernier without MPI being initialized.
TEST(TestMPINotInit, MpiNotInitialised) {

  [[maybe_unused]] int ierr;
  EXPECT_THROW(meto::vernier.init(), std::runtime_error);
  meto::vernier.finalize();

}

