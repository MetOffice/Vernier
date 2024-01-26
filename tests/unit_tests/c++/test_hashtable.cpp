/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#include "vernier.h"

using ::testing::AllOf;
using ::testing::An;
using ::testing::Gt;

int const tid = 0;
std::string tid_str(std::to_string(tid));
std::string tid_bytes(reinterpret_cast<char const*>(&tid), sizeof(tid));

//
//  Testing that the hashing function works as expected (we don't want
//  collisions), and that walltimes are being updated by vernier.stop().
//  The desired behaviour of calling get_total_walltime before vernier.stop()
//  is a bit fuzzy at the time of writing, but currently a test is done to make
//  sure it returns the MDI of 0.0
//

TEST(HashTableTest,HashFunctionTest) {

  // Create new hashes via HashTable::query_insert, which is used in Vernier::start
  const auto& prof_rigatoni = meto::vernier.start("Rigatoni");
  const auto& prof_penne    = meto::vernier.start("Penne");
  meto::vernier.stop(prof_penne);
  meto::vernier.stop(prof_rigatoni);

  {
    SCOPED_TRACE("Hashing related fault");

    // Checking that:
    //  - query_insert'ing Penne or Rigatoni just returns the hash
    //  - the regions have different hashes
    //  - the regions have the hashes returned by hash_function_ which uses std::hash
    EXPECT_EQ(meto::vernier.start("Rigatoni"), std::hash<std::string_view>{}("Rigatoni" + tid_bytes));
    EXPECT_EQ(meto::vernier.start("Penne"),    std::hash<std::string_view>{}("Penne" + tid_bytes));
  }

}

/**
 * @TODO  Decide how to handle the MDI stuff and update the following test
 *        accordingly. See Issue #53.
 *
 */

TEST(HashTableTest,UpdateTimesTest) {

  // Create new hash
  size_t prof_pie = std::hash<std::string>{}("Pie" + tid_bytes);

  // Trying to find a time before .start() will throw an exception
  EXPECT_THROW(meto::vernier.get_total_walltime(prof_pie, 0), std::out_of_range);

  // Start timing
  auto const& expected_hash = meto::vernier.start("Pie");
  EXPECT_EQ(expected_hash,prof_pie); // Make sure prof_pie has the hash we expect

  sleep(1);

  // Time t1 declared inbetween .start() and first .stop()
  double const t1 = meto::vernier.get_total_walltime(prof_pie, 0);

  //Stop timing
  meto::vernier.stop(prof_pie);

  // Time t2 declared after first vernier.stop()
  double const t2 = meto::vernier.get_total_walltime(prof_pie, 0);

  // Start and stop same region again
  meto::vernier.start("Pie");
  sleep(1);
  meto::vernier.stop(prof_pie);

  // Time t3 declared after second vernier.stop()
  double const t3 = meto::vernier.get_total_walltime(prof_pie, 0);

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
