#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::ExitedWithCode;
using ::testing::KilledBySignal;

//
//  Tests and death tests more related to profiler class members. WriteTest
//  checks 'hashvec' in profiler.write() is correctly ordered. One death test
//  makes sure the code breaks when a hash mismatch happens, and the other tests
//  for a segfault when stopping before anything else.
//

TEST(ProfilerTest,WriteTest) {

  // Start 3 nested regions
  const auto& prof_shortbread = prof.start("Shortbread");
  const auto& prof_brownie    = prof.start("Brownie");
  const auto& prof_rockyroad  = prof.start("RockyRoad");

  // Stop them 1 by 1
  prof.stop(prof_rockyroad);
  prof.stop(prof_brownie);
  prof.stop(prof_shortbread);

  // Call write to ensure hashvec is filled & sorted
  prof.write();

  // Get the Hashvec
  const auto& local_hashvec = prof.get_hashvec(0);

  {
    SCOPED_TRACE("Hashvec has incorrect size!");

    // Since hashvec is a vector of (hash,HashEntry) pairs
    // it should be equal in size to the number of regions
    ASSERT_EQ(local_hashvec.size(), 3);
  }

  {
    SCOPED_TRACE("Entries in hashvec incorrectly sorted");

    // hashvec is ordered from high to low so... [lastEntry] < [firstEntry]
    const double& val1 = local_hashvec[0].second.self_walltime_.count();
    const double& val2 = local_hashvec[1].second.self_walltime_.count();
    const double& val3 = local_hashvec[2].second.self_walltime_.count();

    EXPECT_LT(val3, val2);
    EXPECT_LT(val2, val1);
  }
}

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
