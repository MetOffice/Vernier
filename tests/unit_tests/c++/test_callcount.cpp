/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <omp.h>

#include "profiler.h"

TEST(HashEntryTest,CallCountTest)
{
  // Start main region
  auto prof_main = prof.start("MainRegion");

  // Declare a shared sub-region hash. Initialise num_threads so that the
  // compiler knows the 'for' loop inside the parallel region will definitely
  // happen, and therefore doesn't think prof_sub_private remains unitialised.
  size_t prof_sub_shared;
  int num_threads = 1;

  // Start parallel region
#pragma omp parallel default(none) shared(prof_sub_shared, prof, num_threads)
  {
    // Get total number of threads, only need to calculate on a single thread
    // since value won't change.
#pragma omp single
    {
      num_threads = omp_get_num_threads();
    }

    // Current thread ID
    int thread_id  = omp_get_thread_num();

    // Also initialise prof_sub_private. The compiler doesn't know how many
    // iterations of the subsequent 'for' loop there will be, and may flag
    // warnings about it being potentially uninitialised when assigning to
    // prof_sub_shared.
    size_t prof_sub_private = 0;

    // Call a subregion a differing number of times depending on the thread ID.
    // The highest thread ID will have the fewest calls: just 1.
    for (int i = 0; i < num_threads-thread_id; ++i)
    {
      prof_sub_private = prof.start("SubRegion");
      prof.stop(prof_sub_private);
    }

    // Give prof_sub_shared a value for later use in EXPECT's
#pragma omp single
    {
      prof_sub_shared = prof_sub_private;
    }
  }

  // Stop main region
  prof.stop(prof_main);

  // Check call_count_ is the number expected on all threads. On most threads,
  // the profiler calliper call count should match this number, except on thread
  // zero which includes the main region callipers.
  for (int j = 0; j < num_threads; ++j)
  {
    EXPECT_EQ(prof.get_call_count(prof_sub_shared,j),num_threads-j);

    int incr = (j==0) ? 1 : 0;
    EXPECT_EQ(prof.get_prof_call_count(j),num_threads-j+incr);
  }

}
