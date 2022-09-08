/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>

#include "profiler.h"

#include "omp.h"
#include <unistd.h>

TEST(SystemTests, TimingTest)
{

  // Start timing: noddy way, and using Profiler.
  auto prof_main = prof.start("MAIN");
  double t1 = omp_get_wtime();

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

  // End of profiling; record t2 immediately before.
  double t2 = omp_get_wtime();
  prof.stop(prof_main);

  // Write the profile
  prof.write();

  // Check that the total time measured by the profiler is within some tolerance
  // of the actual time measured by simple t2-t1.  This only tests the top-level
  // timing, not individual subroutine timings.
  double const time_tolerance = 0.0001;

  double actual_time = t2 - t1;
  double prof_time  = prof.get_thread0_walltime(prof_main);
  EXPECT_NEAR(prof_time, actual_time, time_tolerance);

  std::cout << "\n" << "Actual timing: "   << actual_time << "\n";
  std::cout << "\n" << "Profiler timing: " << prof_time  << "\n\n";

}
