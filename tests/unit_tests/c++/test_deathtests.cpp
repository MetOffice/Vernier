#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;

//
//  Some death tests that didn't fit nicely into other files and are hence grouped
//  together here. One makes sure the code breaks when a hash mismatch happens,
//  and the other tests for a segfault when stopping before anything else.
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

    const auto& prof_main = prof.hashtable_query_insert("Main");

    // Stop the profiler before anything is done
    prof.stop(prof_main);

  }, "" );

}
