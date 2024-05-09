/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <mpi.h>

#include "vernier.h"
#include "error_handler.h"
#include "hashvec_handler.h"

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;

//
//  Tests and death tests related to profiler class members.
//

// Make sure the code exits when a hash mismatch happens.
TEST(ProfilerDeathTest,WrongHashTest) {

EXPECT_EXIT({
    MPI_Init(NULL,NULL);

    // Start main
    const auto& prof_main = meto::vernier.start("Chocolate");

    // A subregion
    const auto& prof_sub = meto::vernier.start("Vanilla");
    meto::vernier.stop(prof_sub);

    // Wrong hash in profiler.stop()
    meto::vernier.stop(prof_sub);

    // Eventually stop prof_main to avoid Wunused telling me off...
    meto::vernier.stop(prof_main);

    MPI_Finalize();
  }, ExitedWithCode(100), "EMERGENCY STOP: hashes don't match.");
  
}

// Tests for a segfault when stopping before anything else.
TEST(ProfilerDeathTest,StopBeforeStartTest) {

  EXPECT_EXIT({
    MPI_Init(NULL,NULL);

    const auto prof_main = std::hash<std::string_view>{}("Main");

    // Stop the profiler before anything is done
    meto::vernier.stop(prof_main);

    MPI_Finalize();
  }, ExitedWithCode(101), "EMERGENCY STOP: stop called before start calliper.");

}

// The traceback array is not a growable vector. Check that the code exits
// when available array elements are exhaused.
TEST(ProfilerDeathTest, TooManyTracebackEntries) {

  EXPECT_EXIT({
    MPI_Init(NULL,NULL);

    const int beyond_maximum = PROF_MAX_TRACEBACK_SIZE+1;
    for (int i=0; i<beyond_maximum; ++i){
      [[maybe_unused]] auto prof_handle = meto::vernier.start("TracebackEntry");
    }

    MPI_Finalize();
  }, ExitedWithCode(102), "EMERGENCY STOP: Traceback array exhausted.");

}

//Tests the correct io mode is set. If not set correctly it will exit.
TEST(ProfilerDeathTest, InvalidIOModeTest) {
  EXPECT_EXIT({
    MPI_Init(NULL,NULL);
    const char *invalidIOMode = "single";
    setenv("VERNIER_OUTPUT_MODE", invalidIOMode, 1);

    meto::HashVecHandler();

    MPI_Finalize();
  }, ExitedWithCode(EXIT_FAILURE), "Invalid IO mode choice");
}