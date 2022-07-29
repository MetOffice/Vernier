#include <iostream>
#include <profiler.h>

#include <gtest/gtest.h> 
#include <gmock/gmock.h>
using ::testing::ExitedWithCode;
using ::testing::WhenSortedBy;
using ::testing::ElementsAre;
using ::testing::An;
using ::testing::Gt;
using ::testing::Not;
using ::testing::StrCaseEq;
using ::testing::KilledBySignal;

// USing global profiler declared at the bottom of "profiler.h" :
//
//            inline Profiler prof;
//

TEST(HashTableTest,QueryInsertTest) { 

   // Nothing has been done yet, table should be empty
   EXPECT_TRUE(prof.is_table_empty());

   // Create new hashes via query insert
   const auto& prof_main = prof.hashtable_query_insert("Rigatoni"); 
   const auto& prof_sub = prof.hashtable_query_insert("Penne");

   // Table no longer empty
   EXPECT_FALSE(prof.is_table_empty());

   // .count() returns 1 if map already has an entry associated with input hash
   EXPECT_EQ(prof.get_hashtable_count(prof_main), 1);
   EXPECT_EQ(prof.get_hashtable_count(prof_sub),  1);

   // Hash will be of type size_t
   EXPECT_THAT(prof_main, An<size_t>());
   EXPECT_THAT(prof_sub,  An<size_t>());

   // Try to query_insert penne or rigatoni, it should just return the hash 
   EXPECT_EQ(prof.hashtable_query_insert("Rigatoni"), prof_main);
   EXPECT_EQ(prof.hashtable_query_insert("Penne"),    prof_sub) ;

   // Checking hash_function_ and making sure regions are different 
   EXPECT_EQ(prof_main, std::hash<std::string_view>{}("Rigatoni"));
   EXPECT_NE(prof_sub , std::hash<std::string_view>{}("Rigatoni")); 

   // HashTable.query_insert() includes "noexcept" specifier
   EXPECT_NO_THROW( prof.hashtable_query_insert("Penne") );

}

TEST(HashTableTest,ThreadsEqualsEntries) { 

   // Start profiler timing 
   const auto& prof_main = prof.start("MAIN"); 

   // Time something
   sleep(1);
   
   // Stop timing  
   prof.stop(prof_main); 

   // Compare number of threads and number of hashtable entries 
   EXPECT_EQ( prof.get_max_threads() , prof.get_thread_hashtables_size() ); 
   EXPECT_EQ( prof.get_max_threads() , prof.get_thread_traceback_size()  );
   // NOTE: .size() returns size_t so am I allowed to compare types int & size_t like this?

}

TEST(HashTableTest,UpdateTest) { 

   // Create new hash 
   auto prof_main = prof.hashtable_query_insert("Pie");

   // Initial time for this hash should be zero
   const double& t1 = prof.get_thread0_walltime(prof_main);
   EXPECT_DOUBLE_EQ(t1, 0.0);

   // Start timing 
   prof.start("Pie"); 

   sleep(1);
   const double& t2 = prof.get_thread0_walltime(prof_main);

   // Stop timing  
   prof.stop(prof_main);

   // Stop
   const double& t3 = prof.get_thread0_walltime(prof_main);

   // Start and stop same region again
   prof.start("Pie"); 
   sleep(1);
   prof.stop(prof_main);
   const double& t4 = prof.get_thread0_walltime(prof_main);

   // Ideally t3 > t2 > t1
   // Instead, t1 = 0.0
   //          t2 = MDI 
   //          t3 > t2/t1
   {
      double MDI = 0.0;     // Missing Data Indicator (MDI)
      EXPECT_EQ(t1, MDI);
      EXPECT_EQ(t2, MDI);
      EXPECT_GT(t4, t3 );
   }
}

TEST(HashTableTest,WriteTest) {

   // Start 3 nested regions  
   const auto& prof_main = prof.start("Shortbread"); 
   const auto& prof_sub  = prof.start("Brownie"); 
   const auto& prof_sub2 = prof.start("RockyRoad"); 

   // Stop them 1 by 1
   sleep(1);
   prof.stop(prof_sub2); 
   sleep(1);
   prof.stop(prof_sub);
   sleep(1);  
   prof.stop(prof_main);

   // TYPE test 
   EXPECT_THAT( prof.get_hashvec()[0].first                 , An<size_t>() );
   EXPECT_THAT( prof.get_hashvec()[0].second.child_walltime_, An<double>() );

   // Since hashvec is a vector of (hash,HashEntry) pairs 
   // it should be equal in size to the number of regions
   {  
     EXPECT_EQ(prof.get_hashvec().size(), 3);
   }

   // hashvec is ordered from high to low so... [lastEntry] < [firstEntry] 
   const double& val1 = prof.get_hashvec()[0].second.self_walltime_;
   const double& val2 = prof.get_hashvec()[1].second.self_walltime_;
   const double& val3 = prof.get_hashvec()[2].second.self_walltime_;
   EXPECT_LT(val3, val2);
   EXPECT_LT(val2, val1);
                                                                                                                                                                                             
   // Another test that elements of hashvec are indeed ordered from high to low selftime
   const double container1[3] = { prof.get_thread0_self_walltime(prof_main) ,
                                  prof.get_thread0_self_walltime(prof_sub)  ,
                                  prof.get_thread0_self_walltime(prof_sub2) };
   EXPECT_THAT(container1, WhenSortedBy(std::greater(),ElementsAre(val1, val2, val3)));
  
}

TEST(HashTableTest,TracebackTest) {

   // Instantiated
   //const auto& prof_main = prof.hashtable_query_insert("Americano");

   // Start profiler
   const auto& prof_main = prof.start("Americano");
   
   EXPECT_EQ( prof.get_final_traceback_pair().first, prof_main );
   EXPECT_THAT( prof.get_final_traceback_pair().second, AllOf(An<double>(),Gt(0.0)) );

   // Start subregion
   const auto& prof_sub = prof.start("Latte");

   EXPECT_EQ( prof.get_final_traceback_pair().first, prof_sub );

   // Stop subregion
   prof.stop(prof_sub);

   EXPECT_EQ( prof.get_final_traceback_pair().first, prof_main );
  
   // Stop profiler
   prof.stop(prof_main);

   //// In this example, after stopping the main profiler we expect an 
   //// error, as the only hash that existed in the vector of pairs was 
   //// just deleted by .pop_back() in profiler.stop()

   EXPECT_EXIT(prof.get_final_traceback_pair().second, ExitedWithCode(11), "seg");
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

     





