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

  prof.stop(prof_main);
  prof.write();

  // Compute t1-t2 timing and output that.
  double t2 = omp_get_wtime();
  std::cout << "\n" << "Timing: " << t2 - t1 << "\n\n";

}
