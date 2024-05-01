/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#ifdef _OPENMP
  #include <omp.h>
#endif

#include "vernier.h"
#include "vernier_get_wtime.h"

int const max_depth = 3;
int const sleep_seconds = 1;

// The time tolerance can be reasonably loose. If the total times were incorrect
// as a result of mis-handled recursion, they would be too large by multiples of
// sleep_seconds.
double const time_tolerance = 0.001;

// ------------------------------------------------------------------------------
//  Structs
// ------------------------------------------------------------------------------

// Structure To hold timings from the various recursive functions.
struct Timings
{
  double zeroth_function_total_time_ = 0.0;
  double first_function_total_time_  = 0.0;
  double second_function_total_time_ = 0.0;
};

// ------------------------------------------------------------------------------
//  Forward declarations
// ------------------------------------------------------------------------------

// Needed to escape circular dependence on function declarations.
void second_function(Timings&);

// ------------------------------------------------------------------------------
//  Zeroth function - calls itself
// ------------------------------------------------------------------------------

void zeroth_function(Timings& timings)
{

  static int recursion_depth = 1;
#pragma omp threadprivate(recursion_depth)

  auto prof_handle = meto::vernier.start("first_function");

  sleep(sleep_seconds);
  ++recursion_depth;

  if (recursion_depth <= max_depth)
  {
    zeroth_function(timings);
  }

  meto::vernier.stop(prof_handle);

  // Update the total walltime so far spent in this function.
  int tid = 0;   //const was here before -> int const tid = 0;
  #ifdef _OPENMP
    tid = omp_get_thread_num();
  #endif
  timings.zeroth_function_total_time_ = meto::vernier.get_total_walltime(prof_handle, tid);

}

// ------------------------------------------------------------------------------
//  First function - calls the second function.
// ------------------------------------------------------------------------------

void first_function(Timings& timings)
{

  static int recursion_depth = 1;
#pragma omp threadprivate(recursion_depth)

  auto prof_handle = meto::vernier.start("first_function");

  sleep(sleep_seconds);
  ++recursion_depth;

  if (recursion_depth <= max_depth)
  {
    second_function(timings);
  }

  meto::vernier.stop(prof_handle);

  // Update the total walltime so far spent in this function. Do here while we
  // have access to the prof_handle.
  int tid = 0;   //const was here before -> int const tid = 0;
  #ifdef _OPENMP
    tid = omp_get_thread_num();
  #endif
  timings.first_function_total_time_ = meto::vernier.get_total_walltime(prof_handle, tid);

}

// ------------------------------------------------------------------------------
//  Second function - calls the first function.
// ------------------------------------------------------------------------------

void second_function(Timings& timings)
{
  static int recursion_depth = 1;
#pragma omp threadprivate(recursion_depth)

  auto prof_handle = meto::vernier.start("second_function");

  sleep(sleep_seconds);
  ++recursion_depth;

  if (recursion_depth <= max_depth)
  {
    first_function(timings);
  }

  meto::vernier.stop(prof_handle);

  // Update the total walltime so far spent in this function. Do here while we
  // have access to the prof_handle.
  int tid = 0;         //const was here before -> int const tid = 0;
  #ifdef _OPENMP
    tid = omp_get_thread_num();
  #endif
  timings.second_function_total_time_ = meto::vernier.get_total_walltime(prof_handle, tid);
}

// -------------------------------------------------------------------------------
//  Main tests
// -------------------------------------------------------------------------------

//
// Direct recursion
//

TEST(RecursionTest,DirectRecursion)
{

  meto::vernier.init();
  auto prof_handle = meto::vernier.start("test_recursion");

  // Test independently on each thread.
#pragma omp parallel
  {
    auto prof_handle_threaded = meto::vernier.start("test_recursion:threads");

    Timings timings;
    double t1 = meto::vernier_get_wtime();

    // Function calls itself
    zeroth_function(timings);

    double t2 = meto::vernier_get_wtime();
    double overall_time = t2-t1;

    EXPECT_LE  (timings.zeroth_function_total_time_,  overall_time);
    EXPECT_NEAR(timings.zeroth_function_total_time_,  overall_time, time_tolerance);

    meto::vernier.stop(prof_handle_threaded);
  }

  meto::vernier.stop(prof_handle);
  meto::vernier.finalize();
}

//
// Indirect recursion
//

TEST(RecursionTest,IndirectRecursion)
{
  meto::vernier.init();
  auto prof_handle = meto::vernier.start("test_recursion");

  // Test independently on each thread.
#pragma omp parallel
  {
    auto prof_handle_threaded = meto::vernier.start("test_recursion:threads");

    Timings timings;
    double t1 = meto::vernier_get_wtime();

    // Function calls a second function
    first_function(timings);

    double t2 = meto::vernier_get_wtime();

    double overall_time = t2-t1;
      
    EXPECT_LE  (timings.first_function_total_time_,  overall_time);
    EXPECT_NEAR(timings.first_function_total_time_,  overall_time, time_tolerance);
    EXPECT_NEAR(timings.second_function_total_time_, timings.first_function_total_time_ - sleep_seconds, time_tolerance);

    meto::vernier.stop(prof_handle_threaded);
  }

  meto::vernier.stop(prof_handle);
  meto::vernier.finalize();
}

