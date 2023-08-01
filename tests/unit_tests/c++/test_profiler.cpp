/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <omp.h>
#include <unistd.h>

#include "vernier.h"

TEST(SystemTests, TimingTest)
{
  // Start timing: noddy way, and using Vernier.
  auto prof_main = meto::vernier.start("MAIN");
  double t1 = omp_get_wtime();

  // Time a region
  {
    auto prof_sub = meto::vernier.start("MAIN_SUB");
    sleep(1);
    meto::vernier.stop(prof_sub);
  }

  // Time nested regions on many threads.
#pragma omp parallel
  {
    auto prof_sub = meto::vernier.start("MAIN_SUB");
    sleep(1);

    // Time nested region
    auto prof_sub2 = meto::vernier.start("MAIN_SUB2");
    sleep(1);
    meto::vernier.stop(prof_sub2);

    // Outer region end.
    meto::vernier.stop(prof_sub);
  }

  // Give the main regions some substantial execution time.
  sleep(2);

  // End of profiling; record t2 immediately before.
  double t2 = omp_get_wtime();
  meto::vernier.stop(prof_main);

  // Check that the total time measured by the profiler is within some tolerance
  // of the actual time measured by simple t2-t1.  This only tests the top-level
  // timing, not individual subroutine timings.
  double const time_tolerance = 0.0005;

  double actual_time = t2 - t1;
  double prof_time  = meto::vernier.get_total_walltime(prof_main, 0);
  std::cout << "\n" << "Actual timing: "   << actual_time << "\n";
  std::cout << "\n" << "Profiler timing: " << prof_time  << "\n\n";
  EXPECT_NEAR(prof_time, actual_time, time_tolerance);


}
