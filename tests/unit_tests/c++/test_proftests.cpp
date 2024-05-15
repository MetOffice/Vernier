/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>

#include "vernier.h"
#include "vernier_mpi.h"
#include "error_handler.h"
#include "hashvec_handler.h"

using ::testing::ExitedWithCode;

//
//  Tests and death tests related to profiler class members.
//

// Make sure the code exits when a hash mismatch happens.
TEST(ProfilerDeathTest,WrongHashTest) {

  meto::vernier.init();

  EXPECT_EXIT({

    // Start main
    const auto& prof_main = meto::vernier.start("Chocolate");

    // A subregion
    const auto& prof_sub = meto::vernier.start("Vanilla");
    meto::vernier.stop(prof_sub);

    // Wrong hash in profiler.stop()
    meto::vernier.stop(prof_sub);

    // Eventually stop prof_main to avoid Wunused telling me off...
    meto::vernier.stop(prof_main);

  }, ExitedWithCode(100), "EMERGENCY STOP: hashes don't match.");

  meto::vernier.finalize();
}

// Tests for a segfault when stopping before anything else.
TEST(ProfilerDeathTest,StopBeforeStartTest) {

  EXPECT_EXIT({

    const auto prof_main = std::hash<std::string_view>{}("Main");

    // Stop the profiler before anything is done
    meto::vernier.stop(prof_main);

  }, ExitedWithCode(101), "EMERGENCY STOP: stop called before start calliper.");
}

// Vernier is not initialised before first start() call.
TEST(ProfilerDeathTest, StartBeforeInit) {

  EXPECT_EXIT({
    meto::vernier.start("MAIN");
  }, 
  ExitedWithCode(EXIT_FAILURE), 
  "Vernier::start_part1. Vernier not initialised.");
}

// MPI is initialised, but the passed communicator handle is
// MPI_COMM_NULL.
TEST(ProfilerDeathTest, NullCommunicatorPassed) {
  [[maybe_unused]] int ierr;

  EXPECT_EXIT({
    meto::vernier.init(MPI_COMM_NULL);
  },
  ExitedWithCode(EXIT_FAILURE),
  "MPIContext::init. MPI initialized, but null communicator passed.");

  meto::vernier.finalize();
}

// Check that uninitialised MPI is caught in the write functionality.
TEST(ProfilerDeathTest, VernierUninitialisedInWrite) {

  // No init() called yet, so MPI context not initialised.
  
  EXPECT_EXIT({
    meto::vernier.write();
  },
  ExitedWithCode(EXIT_FAILURE),
  "Vernier::write. Vernier not initialised.");
}

// The traceback array is not a growable vector. Check that the code exits
// when available array elements are exhausted.
TEST(ProfilerDeathTest, TooManyTracebackEntries) {

  meto::vernier.init();

  EXPECT_EXIT({

    const int beyond_maximum = PROF_MAX_TRACEBACK_SIZE+1;
    for (int i=0; i<beyond_maximum; ++i){
      [[maybe_unused]] auto prof_handle = meto::vernier.start("TracebackEntry");
    }

  }, ExitedWithCode(102), "EMERGENCY STOP: Traceback array exhausted.");

  meto::vernier.finalize();
}

//Tests the correct io mode is set. If not set correctly it will exit.
TEST(ProfilerDeathTest, InvalidIOModeTest) {
  EXPECT_EXIT({

    meto::MPIContext mpi_context;

    const char *invalidIOMode = "single";
    setenv("VERNIER_OUTPUT_MODE", invalidIOMode, 1);

    meto::HashVecHandler object(mpi_context);

  }, ExitedWithCode(EXIT_FAILURE), "Invalid IO mode choice");
}
