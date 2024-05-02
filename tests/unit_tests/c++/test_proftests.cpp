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

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;


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

  EXPECT_DEATH({

    const auto prof_main = std::hash<std::string_view>{}("Main");

    // Stop the profiler before anything is done
    meto::vernier.stop(prof_main);

  }, "" );
}

// Vernier is not initialised before first start() call.
TEST(ProfilerDeathTest, StartBeforeInit) {
  EXPECT_THROW(meto::vernier.start("MAIN"), std::runtime_error);
}

// MPI is initialised, but the passed communicator handle is
// MPI_COMM_NULL.
TEST(ProfilerDeathTest, NullCommunicatorPassed) {
  [[maybe_unused]] int ierr;
  EXPECT_THROW(meto::vernier.init(MPI_COMM_NULL), std::runtime_error);
  meto::vernier.finalize();
}

// Check that uninitialised MPI is caught in the write functionality.
TEST(ProfilerDeathTest, VernierUninitialisedInWrite) {

  // No init() called yet, so MPI context not initialised.
  EXPECT_THROW(meto::vernier.write(), std::runtime_error);

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
