#include <iostream>
#include <profiler.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::An;
using ::testing::Gt;

//
//  Tesing some hashtable member variables and functions, such as query_insert()
//  and the std::vector thread_traceback_. The desired behaviour of calling
//  get_thread0_walltime before profiler.stop() is a bit fuzzy at the time of
//  writing, but currently a test is done to make sure it returns the MDI of 0.0
//

TEST(HashTableTest,QueryInsertTest) {

  {
    SCOPED_TRACE("Table not empty beforehand");

    // Nothing has been done yet, table should be empty
    EXPECT_TRUE(prof.is_table_empty());
  }

  // Create new hashes via query insert
  const auto& prof_main = prof.hashtable_query_insert("Rigatoni");
  const auto& prof_sub  = prof.hashtable_query_insert("Penne");

  {
    SCOPED_TRACE("Table still empty or .count() returning 0 after query_insert");

    // Table no longer empty
    EXPECT_FALSE(prof.is_table_empty());

    // .count() returns 1 if map already has an entry associated with input hash
    EXPECT_EQ(prof.get_hashtable_count(prof_main), 1);
    EXPECT_EQ(prof.get_hashtable_count(prof_sub),  1);
  }

  {
    SCOPED_TRACE("Hashing related fault");

    // Checking that:
    //  - query_insert'ing Penne or Rigatoni just returns the hash
    //  - the regions have different hashes
    //  - the regions have the hashes returned by hash_function_ which uses std::hash
    EXPECT_EQ(prof.hashtable_query_insert("Rigatoni"), std::hash<std::string_view>{}("Rigatoni"));
    EXPECT_EQ(prof.hashtable_query_insert("Penne"),    std::hash<std::string_view>{}("Penne"));
  }

  // HashTable.query_insert() includes "noexcept" specifier
  EXPECT_NO_THROW( prof.hashtable_query_insert("Penne") );

}

TEST(HashTableTest,ThreadsEqualsEntries) {

  // Start and stop profiler timing
  const auto& prof_main = prof.start("MAIN");
  prof.stop(prof_main);

  // Compare number of threads and number of hashtable entries
  EXPECT_EQ( prof.get_max_threads() , prof.get_thread_hashtables_size() );
  EXPECT_EQ( prof.get_max_threads() , prof.get_thread_traceback_size()  );

}

TEST(HashTableTest,UpdateAndMdiTest) {

  // Create new hash
  auto prof_main = prof.hashtable_query_insert("Pie");

  // Time t1 declared before profiler.start()
  const double& t1 = prof.get_thread0_walltime(prof_main);

  // Start timing
  prof.start("Pie");

  sleep(1);

  // Time t2 declared inbetween .start() and first .stop()
  const double& t2 = prof.get_thread0_walltime(prof_main);

  // Stop timing
  prof.stop(prof_main);

  // Time t3 declared after first profiler.stop()
  const double& t3 = prof.get_thread0_walltime(prof_main);

  // Start and stop same region again
  prof.start("Pie");
  sleep(1);
  prof.stop(prof_main);

  // Time t4 declared after second profiler.stop()
  const double& t4 = prof.get_thread0_walltime(prof_main);

  // Expected behaviour: t1 & t2 return the MDI and t4 > t3 > 0
  constexpr double MDI = 0.0;     // Missing Data Indicator (MDI)

  {
    SCOPED_TRACE("MDI missing from time points expected to return it");
    EXPECT_EQ(t1, MDI);
    EXPECT_EQ(t2, MDI);

    // Introduce a subregion hash but never time it.
    // The time for this region should also return an MDI.
    const auto& prof_sub = prof.hashtable_query_insert("Sub");
    const double& subregionTime = prof.get_thread0_walltime(prof_sub);
    EXPECT_EQ(subregionTime, MDI);
  }

  {
    SCOPED_TRACE("Update potentially not incrementing times correctly");
    EXPECT_GT(t3, 0.0);
    EXPECT_GT(t4, t3 );
  }
}

TEST(HashTableTest,TracebackTest) {

  // Instantiate hashtable
  const auto& prof_main = prof.hashtable_query_insert("Main");

  {
    SCOPED_TRACE("traceback.at() not throwing exception before profiler.start()");

    // .at() throws exception when trying to access entry which isn't there
    EXPECT_ANY_THROW(prof.get_traceback_vector().at(0) );
  }

  // Start profiler
  prof.start("Main");

  {
    SCOPED_TRACE("Traceback vector setup incorrectly");

    // The traceback vector for this thread is a vector of pairs of (hash,StartTime)
    // Therefore...
    //  - It should have a size of 1 in this example
    //  - The first entry in the pair should be prof_main
    //  - The second entry is some time, here a check done to make sure it is some double greater than zero
    EXPECT_EQ( prof.get_traceback_vector().size(), 1);
    EXPECT_EQ( prof.get_traceback_vector().back().first, prof_main );
    EXPECT_THAT( prof.get_traceback_vector().back().second, AllOf(An<double>(),Gt(0.0)) );
  }

  // Stop profiler
  prof.stop(prof_main);

  {
    SCOPED_TRACE("Traceback vector not empty, pop_back() failed or still an unexpected entry left?");

    // Shouldn't be any elements left
    EXPECT_TRUE(prof.get_traceback_vector().empty() );
  }

  {
    SCOPED_TRACE("traceback.at() not throwing exception after element is deleted by pop_back()");

    // .at() should throw exception again, only existing element was deleted by .pop_back() inside profiler.stop()
    EXPECT_ANY_THROW(prof.get_traceback_vector().at(0) );
  }
}
