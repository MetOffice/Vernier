/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <chrono>

#include "profiler.h"

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
  const auto chrono_main_start = std::chrono::steady_clock::now();

  sleep(1);

  // Start a sub-region and chrono timing
  const auto& prof_sub = prof.start("SalmonQuiche");
  const auto chrono_sub_start = std::chrono::steady_clock::now();

  sleep(1);

  // Stop profiler sub-region and respective chrono time
  const auto chrono_sub_end = std::chrono::steady_clock::now();
  prof.stop(prof_sub);

  // Stop profiler main region and respective chrono time
  const auto chrono_main_end = std::chrono::steady_clock::now();
  prof.stop(prof_main);

  {
    SCOPED_TRACE("Self walltime calculation failed");

    // Grab the total, child and self wallclock times
    const double& total_raw = prof.get_total_raw_walltime(prof_main,0);
    const double& total     = prof.get_total_walltime    (prof_main,0);
    const double& child     = prof.get_child_walltime    (prof_main,0);
    const double& self      = prof.get_self_walltime     (prof_main,0);
    const double& overhead  = prof.get_overhead_walltime (prof_main,0);
    std::string   region    = prof.get_region_name       (prof_main,0);
  
    // Test that total
    EXPECT_EQ(total_raw,total-overhead) 
      << "   region: "  << region     << std::endl
      << " overhead: "  << overhead   << std::endl
      << "total_raw: "  << total_raw  << std::endl
      << "    total: "  << total      << std::endl;

    // Test that self_walltime = total_walltime - child_walltime
    EXPECT_EQ(self,total_raw-child)
      << "   region: "  << region     << std::endl
      << "     self: "  << self       << std::endl
      << "    child: "  << child      << std::endl
      << "total_raw: "  << total_raw  << std::endl;
      ;
  }

  // Work out chrono durations in seconds
  std::chrono::duration<double> main_region_duration = chrono_main_end - chrono_main_start;
  double main_in_s = main_region_duration.count();

  std::chrono::duration<double> sub_region_duration = chrono_sub_end - chrono_sub_start;
  double sub_in_s  = sub_region_duration.count();

  {
    SCOPED_TRACE("Chrono and profiler times not within tolerance");

    // Specify a time tolerance
    const double time_tolerance = 0.0005;

    // Expect profiler & chrono times to be within tolerance
    EXPECT_NEAR( prof.get_total_walltime(prof_main,0), main_in_s, time_tolerance );
    EXPECT_NEAR( prof.get_total_walltime(prof_sub,0) , sub_in_s , time_tolerance );
  }
}
