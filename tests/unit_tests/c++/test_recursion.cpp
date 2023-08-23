/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <omp.h>

#include "vernier.h"

int const max_depth = 3;
int const sleep_seconds = 1;

// ------------------------------------------------------------------------------
//  File scope variables
// ------------------------------------------------------------------------------

struct Timings
{
  static double first_function_total_time ;
  static double second_function_total_time;
};

double Timings::first_function_total_time  = 0.0;
double Timings::second_function_total_time = 0.0;

// ------------------------------------------------------------------------------
//  Forward declarations
// ------------------------------------------------------------------------------

void first_function(Timings&);
void second_function(Timings&);

// ------------------------------------------------------------------------------
//  First test function
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

  int const tid = omp_get_thread_num();
  timings.first_function_total_time = meto::vernier.get_total_walltime(prof_handle, tid);

}

// ------------------------------------------------------------------------------
//  Second test function
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

  int const tid = omp_get_thread_num();
  timings.second_function_total_time = meto::vernier.get_total_walltime(prof_handle, tid);
}

// -------------------------------------------------------------------------------
//  Main test
// -------------------------------------------------------------------------------

TEST(RecursionTest,IndirectRecursion)
{
  auto prof_handle = meto::vernier.start("test_recursion");

  // Test indepedently on each thread.
#pragma omp parallel
  {
    Timings timings;
    double t1 = omp_get_wtime();

    auto prof_handle_threaded = meto::vernier.start("test_recursion:threads");
    first_function(timings);

    double t2 = omp_get_wtime();
    double overall_time = t2-t1;
    double constexpr time_tolerance = 0.0005;
    
    EXPECT_LE  (timings.first_function_total_time,  overall_time);
    EXPECT_NEAR(timings.first_function_total_time,  overall_time, time_tolerance);
    EXPECT_NEAR(timings.second_function_total_time, timings.first_function_total_time - sleep_seconds, time_tolerance);

    meto::vernier.stop(prof_handle_threaded);
  }

  meto::vernier.stop(prof_handle);
}

