/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>

#include "profiler.h"

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;

//
//  Tests and death tests more related to profiler class members. WriteTest
//  checks 'hashvec' in profiler.write() is correctly ordered. One death test
//  makes sure the code breaks when a hash mismatch happens, and the other tests
//  for a segfault when stopping before anything else.
//

TEST(ProfilerDeathTest,WrongHashTest) {

  EXPECT_EXIT({

    // Start main
    const auto& prof_main = prof.start("Chocolate");

    // A subregion
    const auto& prof_sub = prof.start("Vanilla");
    prof.stop(prof_sub);

    // Wrong hash in profiler.stop()
    prof.stop(prof_sub);

    // Eventually stop prof_main to avoid Wunused telling me off...
    prof.stop(prof_main);

  }, ExitedWithCode(100), "EMERGENCY STOP: hashes don't match.");

}

TEST(ProfilerDeathTest,StopBeforeStartTest) {

  EXPECT_DEATH({

    const auto prof_main = std::hash<std::string_view>{}("Main");

    // Stop the profiler before anything is done
    prof.stop(prof_main);

  }, "" );

}
