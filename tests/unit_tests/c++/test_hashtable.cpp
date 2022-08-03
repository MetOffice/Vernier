#include <iostream>
#include <profiler.h>
#include <gtest/gtest.h> 
#include <gmock/gmock.h>
#include <chrono>

// Using global profiler declared at the bottom of "profiler.h" :
//
//           inline Profiler prof;
//

TEST(HashEntryTest, TimingsTest) {

  // Start main profiler region and chrono timing
  const auto chrono_main_start = std::chrono::high_resolution_clock::now();
  const auto& prof_main = prof.start("QuicheLorraine");

  sleep(1);

  // Start a sub-region and chrono timing
  const auto chrono_sub_start = std::chrono::high_resolution_clock::now();
  const auto& prof_sub = prof.start("SalmonQuiche"); 

  sleep(1); 
    
  // Stop profiler sub-region and respective chrono time
  prof.stop(prof_sub);
  const auto chrono_sub_end = std::chrono::high_resolution_clock::now();

  // Stop profiler main region and respective chrono time
  prof.stop(prof_main);
  const auto chrono_main_end = std::chrono::high_resolution_clock::now();   

  {
    SCOPED_TRACE("Self walltime calculation failed");
    
    // Grab the total, child and self wallclock times
    const double& total = prof.get_thread0_walltime(prof_main); 
    const double& child = prof.get_thread0_child_walltime(prof_main);
    const double& self  = prof.get_thread0_self_walltime(prof_main);

    // Test that self_walltime = total_walltime - child_walltime
    EXPECT_EQ(self,total-child);
  }

  // Work out chrono durations in seconds
  std::chrono::duration<double> main_region_duration = chrono_main_end - chrono_main_start;
  const double& main_in_seconds = main_region_duration.count();

  std::chrono::duration<double> sub_region_duration = chrono_sub_end - chrono_sub_start;
  const double& sub_in_seconds = sub_region_duration.count();

  {
    SCOPED_TRACE("Chrono and profiler times not within tolerance");
 
    // Specify a time tolerance
    const double time_tolerance = 0.0001;

    // Expect profiler & chrono times to be within tolerance
    EXPECT_NEAR( prof.get_thread0_walltime(prof_main), main_in_seconds, time_tolerance );
    EXPECT_NEAR( prof.get_thread0_walltime(prof_sub) , sub_in_seconds , time_tolerance );
  }
}

TEST(HashEntryTest,RegionNameTest) {

  //Start main region with name "Cappucino"
  const auto& prof_main = prof.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    const auto& prof_sub = prof.start("Latte");

    // Get subregion name out from profiler and test
    EXPECT_EQ("Latte", prof.get_thread0_region_name(prof_sub));

    prof.stop(prof_sub);
  }
  
  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    EXPECT_EQ("Cappucino", prof.get_thread0_region_name(prof_main));
  }

  prof.stop(prof_main);

}

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

TEST(HashTableTest,UpdateTest) { 

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
  const double MDI = 0.0;     // Missing Data Indicator (MDI)
  
  {
    SCOPED_TRACE("MDI missing from time points expected to return it");  
    EXPECT_EQ(t1, MDI);
    EXPECT_EQ(t2, MDI);
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
    EXPECT_THAT( prof.get_traceback_vector().back().second, testing::AllOf(testing::An<double>(),testing::Gt(0.0)) );
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

TEST(ProfilerTest,WriteTest) {

  // Start 3 nested regions  
  const auto& prof_main = prof.start("Shortbread");
  const auto& prof_sub  = prof.start("Brownie"); 
  const auto& prof_sub2 = prof.start("RockyRoad"); 

  sleep(1);

  // Stop them 1 by 1
  prof.stop(prof_sub2); 
  prof.stop(prof_sub);
  prof.stop(prof_main);

  {
    SCOPED_TRACE("Hashvec has incorrect size!");

    // Since hashvec is a vector of (hash,HashEntry) pairs 
    // it should be equal in size to the number of regions  
    ASSERT_EQ(prof.get_hashvec().size(), 3);
  }
   
  {
    SCOPED_TRACE("Entries in hashvec incorrectly sorted");

    // hashvec is ordered from high to low so... [lastEntry] < [firstEntry] 
    const double& val1 = prof.get_hashvec()[0].second.self_walltime_;
    const double& val2 = prof.get_hashvec()[1].second.self_walltime_;
    const double& val3 = prof.get_hashvec()[2].second.self_walltime_;
    EXPECT_LT(val3, val2);
    EXPECT_LT(val2, val1);
                                                                                                                                                                                            
    // Confirm that elements of hashvec are indeed ordered from high to low self_walltime
    const double container1[3] = { prof.get_thread0_self_walltime(prof_main) ,
                                   prof.get_thread0_self_walltime(prof_sub)  ,
                                   prof.get_thread0_self_walltime(prof_sub2) };
    EXPECT_THAT(container1, testing::WhenSortedBy(std::greater(),testing::ElementsAre(val1, val2, val3)));
  }
}

TEST(ProfilerTest,WrongHashTest) {

  EXPECT_EXIT({

    // Start main
    const auto& prof_main = prof.start("Chocolate");

    // A subregion
    const auto& prof_sub = prof.start("Vanilla");
    prof.stop(prof_sub); 

    // Wrong hash in profiler.stop()
    prof.stop(prof_sub); 

    // Eventually stop prof_main to avoid Wunused telling me off...
    prof.stop(prof_main);

  }, testing::ExitedWithCode(100), "EMERGENCY STOP: hashes don't match."); 

}

TEST(ProfilerTest,StopBeforeStartTest) { 

  EXPECT_EXIT({ 

    const auto& prof_main = prof.hashtable_query_insert("Main");

    prof.stop(prof_main);

  }, testing::KilledBySignal(SIGSEGV), "" );

}
