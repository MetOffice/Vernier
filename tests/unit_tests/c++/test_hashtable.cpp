/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <omp.h>

#include "profiler.h"

using ::testing::AllOf;
using ::testing::An;
using ::testing::Gt;

//
//  Testing that the hashing function works as expected (we don't want
//  collisions), and that walltimes are being updated by profiler.stop().
//  The desired behaviour of calling get_total_walltime before profiler.stop()
//  is a bit fuzzy at the time of writing, but currently a test is done to make
//  sure it returns the MDI of 0.0
//

TEST(HashTableTest,HashFunctionTest) {

  // Create new hashes via HashTable::query_insert, which is used in Profiler::start
  const auto& prof_rigatoni = meto::prof.start("Rigatoni");
  const auto& prof_penne    = meto::prof.start("Penne");
  meto::prof.stop(prof_penne);
  meto::prof.stop(prof_rigatoni);

  {
    SCOPED_TRACE("Hashing related fault");

    // Checking that:
    //  - query_insert'ing Penne or Rigatoni just returns the hash
    //  - the regions have different hashes
    //  - the regions have the hashes returned by hash_function_ which uses std::hash
    EXPECT_EQ(meto::prof.start("Rigatoni"), std::hash<std::string_view>{}("Rigatoni@0"));
    EXPECT_EQ(meto::prof.start("Penne"),    std::hash<std::string_view>{}("Penne@0"));
  }

}

/**
 * @TODO  Decide how to handle the MDI stuff and update the following test
 *        accordingly. See Issue #53.
 *
 */

TEST(HashTableTest,UpdateTimesTest) {

  // Create new hash
  size_t prof_pie = std::hash<std::string_view>{}("Pie@0");

  // Trying to find a time before .start() will throw an exception
  EXPECT_THROW(meto::prof.get_total_walltime(prof_pie, 0), std::out_of_range);

  // Start timing
  auto const& expected_hash = meto::prof.start("Pie");
  EXPECT_EQ(expected_hash,prof_pie); // Make sure prof_pie has the hash we expect

  sleep(1);

  // Time t1 declared inbetween .start() and first .stop()
  double const t1 = meto::prof.get_total_walltime(prof_pie, 0);

  //Stop timing
  meto::prof.stop(prof_pie);

  // Time t2 declared after first profiler.stop()
  double const t2 = meto::prof.get_total_walltime(prof_pie, 0);

  // Start and stop same region again
  meto::prof.start("Pie");
  sleep(1);
  meto::prof.stop(prof_pie);

  // Time t3 declared after second profiler.stop()
  double const t3 = meto::prof.get_total_walltime(prof_pie, 0);

  // Expected behaviour: t1 return the MDI and t3 > t2 > 0
  constexpr double MDI = 0.0;     // Missing Data Indicator (MDI)

  {
    SCOPED_TRACE("MDI missing from time points expected to return it");
    EXPECT_EQ(t1, MDI);
  }

  {
    SCOPED_TRACE("Update potentially not incrementing times correctly");
    EXPECT_GT(t2, 0.0);
    EXPECT_GT(t3, t2 );
  }
}
