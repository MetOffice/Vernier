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

#include "vernier.h"
#include "error_handler.h"

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;

//
//  Tests and death tests related to profiler class members.
//

// Make sure the code exits when a hash mismatch happens.
TEST(ProfilerDeathTest,WrongHashTest) {

  //EXPECT_EXIT({
  EXPECT_DEATH({
    // Start main
    const auto& prof_main = meto::vernier.start("Chocolate");

    // A subregion
    const auto& prof_sub = meto::vernier.start("Vanilla");
    meto::vernier.stop(prof_sub);

    // Wrong hash in profiler.stop()
    meto::vernier.stop(prof_sub);

    // Eventually stop prof_main to avoid Wunused telling me off...
    meto::vernier.stop(prof_main);

  }, ""); 
  //ExitedWithCode(100), "EMERGENCY STOP: hashes don't match.");

}

// Tests for a segfault when stopping before anything else.
TEST(ProfilerDeathTest,StopBeforeStartTest) {

  EXPECT_DEATH({
    const auto prof_main = std::hash<std::string_view>{}("Main");

    // Stop the profiler before anything is done
    meto::vernier.stop(prof_main);

  }, "" );

}

// The traceback array is not a growable vector. Check that the code exits
// when available array elements are exhaused.
TEST(ProfilerDeathTest, TooManyTracebackEntries) {

  EXPECT_DEATH({
    const int beyond_maximum = PROF_MAX_TRACEBACK_SIZE+1;
    for (int i=0; i<beyond_maximum; ++i){
      [[maybe_unused]] auto prof_handle = meto::vernier.start("TracebackEntry");
    }
  }, "");

}