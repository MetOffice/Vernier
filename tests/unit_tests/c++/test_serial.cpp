/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <mpi.h>

#include "vernier.h"

// Make sure the code exits when a hash mismatch happens.
TEST(TestSerial,MpiNotInitialised) {

  [[maybe_unused]] int ierr;
  EXPECT_THROW(meto::vernier.init(MPI_COMM_NULL), std::runtime_error);
  meto::vernier.finalize();

}

