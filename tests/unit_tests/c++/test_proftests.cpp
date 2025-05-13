/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include "error_handler.h"
#include "hashvec_handler.h"
#include "vernier.h"
#include "vernier_mpi.h"

using ::testing::ExitedWithCode;

//
//  Tests and death tests related to Vernier class members.
//

// Make sure the code exits when a hash mismatch happens.
TEST(DeathTest, WrongHashTest) {

  meto::vernier.init();

  EXPECT_EXIT(
      {
        // Start main
        const auto &prof_main = meto::vernier.start("Chocolate");

        // A subregion
        const auto &prof_sub = meto::vernier.start("Vanilla");
        meto::vernier.stop(prof_sub);

        // Wrong hash in Vernier.stop()
        meto::vernier.stop(prof_sub);

        // Eventually stop prof_main to avoid Wunused telling me off...
        meto::vernier.stop(prof_main);
      },
      ExitedWithCode(EXIT_FAILURE), "EMERGENCY STOP: hashes don't match.");

  meto::vernier.finalize();
}

// Tests for a segfault when stopping before anything else.
TEST(DeathTest, StopBeforeStartTest) {

  EXPECT_EXIT(
      {
        const auto prof_main = std::hash<std::string_view>{}("Main");

        // Stop Vernier before anything is done
        meto::vernier.stop(prof_main);
      },
      ExitedWithCode(EXIT_FAILURE),
      "EMERGENCY STOP: stop called before start calliper.");
}

// Vernier is not initialised before first start() call.
TEST(DeathTest, StartBeforeInit) {
  // clang-format off
  EXPECT_EXIT({ meto::vernier.start("MAIN"); }, ExitedWithCode(EXIT_FAILURE),
              "Vernier::start_part1. Vernier not initialised.");
  // clang-format on
}

// MPI is initialised, but the passed communicator handle is
// MPI_COMM_NULL.
TEST(DeathTest, NullCommunicatorPassed) {
  [[maybe_unused]] int ierr;

  EXPECT_EXIT(
      { meto::vernier.init(MPI_COMM_NULL); }, ExitedWithCode(EXIT_FAILURE),
      "MPIContext::init. MPI initialized, but null communicator passed.");

  meto::vernier.finalize();
}

// Check that uninitialised MPI is caught in the write functionality.
TEST(DeathTest, VernierUninitialisedInWrite) {

  // No init() called yet, so MPI context not initialised.
  // clang-format off
  EXPECT_EXIT({ meto::vernier.write(); }, ExitedWithCode(EXIT_FAILURE),
              "Vernier::write. Vernier not initialised.");
  // clang-format on
}

// The traceback array is not a growable vector. Check that the code exits
// when available array elements are exhausted.
TEST(DeathTest, TooManyTracebackEntries) {

  meto::vernier.init();

  EXPECT_EXIT(
      {
        const int beyond_maximum = PROF_MAX_TRACEBACK_SIZE + 1;
        for (int i = 0; i < beyond_maximum; ++i) {
          [[maybe_unused]] auto prof_handle =
              meto::vernier.start("TracebackEntry");
        }
      },
      ExitedWithCode(EXIT_FAILURE),
      "EMERGENCY STOP: Traceback array exhausted.");

  meto::vernier.finalize();
}

// Tests the correct io mode is set. If not set correctly it will exit.
TEST(DeathTest, InvalidIOModeTest) {
  EXPECT_EXIT(
      {
        meto::MPIContext mpi_context;

        const char *invalidIOMode = "single";
        setenv("VERNIER_OUTPUT_MODE", invalidIOMode, 1);

        meto::HashVecHandler object(mpi_context);
      },
      ExitedWithCode(EXIT_FAILURE), "Invalid IO mode choice");
}
