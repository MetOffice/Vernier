#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>

//
//  A "timings" test that has expectations about the profiler walltime.
//  In particular, a test is done to make sure the final selfwalltime is
//  equal to the total walltime minus child walltime. Also, std::chrono
//  is used to time a main and sub-region, it should return durations equal
//  to the profiler total walltimes, within tolerance.
//

TEST(HashEntryTest, TimingsTest) {

  // Start main profiler region and chrono timing
  const auto& prof_main = prof.start("QuicheLorraine");
  const auto chrono_main_start = std::chrono::high_resolution_clock::now();

  sleep(1);

  // Start a sub-region and chrono timing
  const auto& prof_sub = prof.start("SalmonQuiche");
  const auto chrono_sub_start = std::chrono::high_resolution_clock::now();

  sleep(1);

  // Stop profiler sub-region and respective chrono time
  const auto chrono_sub_end = std::chrono::high_resolution_clock::now();
  prof.stop(prof_sub);

  // Stop profiler main region and respective chrono time
  const auto chrono_main_end = std::chrono::high_resolution_clock::now();
  prof.stop(prof_main);

  {
    SCOPED_TRACE("Self walltime calculation failed");

    // Grab the total, child and self wallclock times
    const double& total = prof.get_thread0_walltime(prof_main);
    const double& child = prof.get_thread0_child_walltime(prof_main);
    const double& self  = prof.get_thread0_self_walltime(prof_main);

    // Test that self_walltime = total_walltime - child_walltime
    EXPECT_EQ(self,total-child);
  }

  // Work out chrono durations in seconds
  std::chrono::duration<double> main_region_duration = chrono_main_end - chrono_main_start;
  const double& main_in_seconds = main_region_duration.count();

  std::chrono::duration<double> sub_region_duration = chrono_sub_end - chrono_sub_start;
  const double& sub_in_seconds = sub_region_duration.count();

  {
    SCOPED_TRACE("Chrono and profiler times not within tolerance");

    // Specify a time tolerance
    const double time_tolerance = 0.0001;

    // Expect profiler & chrono times to be within tolerance
    EXPECT_NEAR( prof.get_thread0_walltime(prof_main), main_in_seconds, time_tolerance );
    EXPECT_NEAR( prof.get_thread0_walltime(prof_sub) , sub_in_seconds , time_tolerance );
  }
}
