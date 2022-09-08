#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <omp.h>

using ::testing::AllOf;
using ::testing::An;
using ::testing::Gt;

//
//  Testing some hashtable member variables and functions, such as query_insert()
//  and the std::vector thread_traceback_. The desired behaviour of calling
//  get_thread0_walltime before profiler.stop() is a bit fuzzy at the time of
//  writing, but currently a test is done to make sure it returns the MDI of 0.0
//

TEST(HashTableTest,QueryInsertTest) {

  // Hashtable instance to poke and prod without changing any private data
  const auto& htable = prof.get_hashtable(0);

  // Nothing has been done yet, hashtable should be empty
  EXPECT_TRUE(htable.empty());

  // Create new hashes via HashTable::query_insert, which is used in Profiler::start
  const auto& prof_rigatoni = prof.start("Rigatoni");
  const auto& prof_penne    = prof.start("Penne");
  prof.stop(prof_penne);
  prof.stop(prof_rigatoni);

  {
    SCOPED_TRACE("HashTable still empty after start is called");

    // Table no longer empty
    EXPECT_FALSE(htable.empty());

    // .count() returns 1 if map already has an entry associated with input hash
    EXPECT_EQ(htable.count(prof_rigatoni), 1);
    EXPECT_EQ(htable.count(prof_penne),    1);
  }

  {
    SCOPED_TRACE("Hashing related fault");

    // Checking that:
    //  - query_insert'ing Penne or Rigatoni just returns the hash
    //  - the regions have different hashes
    //  - the regions have the hashes returned by hash_function_ which uses std::hash
    EXPECT_EQ(prof.start("Rigatoni"), std::hash<std::string_view>{}("Rigatoni"));
    EXPECT_EQ(prof.start("Penne"),    std::hash<std::string_view>{}("Penne"));
  }

}

TEST(HashTableTest,ThreadsEqualsEntries) {

  // Trying to access an entry one higher than the value of max_threads_ should
  // throw an exception as it won't exist assuming
  // max_threads_ == thread_hashtables_.size(). This is just a different way of
  // testing the assertion that already exists in the code.

  EXPECT_THROW(prof.get_hashtable(prof.get_max_threads()+1), std::out_of_range);

}

/**
 * @TODO  Decide how to handle the MDI stuff and update the following test
 *        accordingly. See Issue #53.
 *
 */

TEST(HashTableTest,UpdateAndMdiTest) {

  // Create new hash
  size_t prof_pie = std::hash<std::string_view>{}("Pie");

  // Trying to find a time before .start() will throw an exception
  EXPECT_THROW(prof.get_thread0_walltime(prof_pie), std::out_of_range);

  // Start timing
  auto const& expected_hash = prof.start("Pie");
  EXPECT_EQ(expected_hash,prof_pie); // Make sure prof_pie has the hash we expect

  sleep(1);

  // Time t1 declared inbetween .start() and first .stop()
  double const t1 = prof.get_thread0_walltime(prof_pie);

  //Stop timing
  prof.stop(prof_pie);

  // Time t2 declared after first profiler.stop()
  double const t2 = prof.get_thread0_walltime(prof_pie);

  // Start and stop same region again
  prof.start("Pie");
  sleep(1);
  prof.stop(prof_pie);

  // Time t3 declared after second profiler.stop()
  double const t3 = prof.get_thread0_walltime(prof_pie);

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

TEST(HashTableTest,TracebackTest) {

  const auto& traceback_vec = prof.get_inner_traceback_vector(0);

  {
    SCOPED_TRACE("traceback.at() not throwing exception before profiler.start()");

    // .at() throws exception when trying to access entry which isn't there
    EXPECT_THROW( traceback_vec.at(0), std::out_of_range );
  }

  // Start profiler
  const auto& prof_main = prof.start("Main");

  {
    SCOPED_TRACE("Traceback vector setup incorrectly");

    // The traceback vector for this thread is a vector of pairs of (hash,StartTime)
    // Therefore...
    //  - It should have a size of 1 in this example
    //  - The first entry in the pair should be prof_main
    //  - The second entry is some time, here a check done to make sure it has the type "time_point_t"
    EXPECT_EQ( traceback_vec.size(), 1);
    EXPECT_EQ( traceback_vec.back().first, prof_main );
    EXPECT_THAT( traceback_vec.back().second, An<time_point_t>() );
  }

  // Stop profiler
  prof.stop(prof_main);

  {
    SCOPED_TRACE("Traceback vector not empty, pop_back() failed or still an unexpected entry left?");

    // Shouldn't be any elements left
    EXPECT_TRUE( traceback_vec.empty() );
  }

  {
    SCOPED_TRACE("traceback.at() not throwing exception after element is deleted by pop_back()");

    // .at() should throw exception again, only existing element was deleted by .pop_back() inside profiler.stop()
    EXPECT_ANY_THROW( traceback_vec.at(0) );
  }
}
