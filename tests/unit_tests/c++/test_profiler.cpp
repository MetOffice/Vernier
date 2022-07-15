/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <unistd.h>
#include <gtest/gtest.h>

#include "omp.h"

#include "profiler.h"

TEST(SystemTests, TimingTest)
{

  // Start timing: noddy way, and using Profiler.
  double t1 = omp_get_wtime();
  auto prof_main = prof.start("MAIN");

  // Time a region
  {
    auto prof_sub = prof.start("MAIN_SUB");
    sleep(1);
    prof.stop(prof_sub);
  }

  // Time nested regions on many threads.
#pragma omp parallel
  {
    auto prof_sub = prof.start("MAIN_SUB");
    sleep(1);

    // Time nested region
    auto prof_sub2 = prof.start("MAIN_SUB2");
    sleep(1);
    prof.stop(prof_sub2);

    // Outer region end.
    prof.stop(prof_sub);
  }

  // Give the main regions some substantial execution time.
  sleep(2);

  // End of profiling; record t2 immediately afterwards.
  prof.stop(prof_main);
  double t2 = omp_get_wtime();

  // Write the profile
  prof.write();

  // Check that the total time measured by the profiler is within some tolerance
  // of the actual time measured by simple t2-t1.  This only tests the top-level
  // timing, not individual subroutine timings.
  double const time_tolerance = 0.0001;

  double actual_time = t2 - t1;
  double prof_time   = prof.get_total_wallclock_time();
  EXPECT_NEAR(prof_time, actual_time, time_tolerance);

  std::cout << "\n" << "Actual timing: "   << actual_time << "\n";
  std::cout << "\n" << "Profiler timing: " << prof_time  << "\n\n";

}

