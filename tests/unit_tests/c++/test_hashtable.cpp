#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

#include <gmock/gmock.h>
using ::testing::An;
using ::testing::ExitedWithCode;

static Profiler test_profiler;

TEST(HashTableTest,QueryInsertTest) { 

   // Nothing has been done yet, table should be empty
   EXPECT_TRUE(test_profiler.is_table_empty());

   // Create new hashes via query insert
   auto prof_main = test_profiler.hashtable_query_insert("Rigatoni"); 
   auto prof_sub = test_profiler.hashtable_query_insert("Penne");

   // Table no longer empty
   EXPECT_FALSE(test_profiler.is_table_empty());

   // .count() returns 1 if map already has an entry associated with input hash
   EXPECT_EQ(test_profiler.get_hashtable_count(prof_main), 1);
   EXPECT_EQ(test_profiler.get_hashtable_count(prof_sub),  1);

   // Hash will be of type size_t
   EXPECT_THAT(prof_main, An<size_t>());
   EXPECT_THAT(prof_sub,  An<size_t>());

   // Try to query_insert penne or rigatoni, it should just return the hash 
   EXPECT_EQ(test_profiler.hashtable_query_insert("Rigatoni"), prof_main);
   EXPECT_EQ(test_profiler.hashtable_query_insert("Penne"),    prof_sub) ;

   // Checking hash_function_ and making sure regions are different 
   EXPECT_EQ(prof_main, std::hash<std::string_view>{}("Rigatoni"));
   EXPECT_NE(prof_sub , std::hash<std::string_view>{}("Rigatoni")); 

   // HashTable.query_insert() includes "noexcept" specifier
   EXPECT_NO_THROW( test_profiler.hashtable_query_insert("Penne") );

}

TEST(HashTableTest,ThreadsEqualsEntries) { 

   // Start profiler timing 
   auto prof_main = test_profiler.start("MAIN"); 

   // Time something
   sleep(1);
   
   // Stop timing  
   test_profiler.stop(prof_main); 

   // Compare number of threads and number of hashtable entries 
   EXPECT_EQ( test_profiler.get_max_threads() , test_profiler.get_thread_hashtables_size() ); 
   EXPECT_EQ( test_profiler.get_max_threads() , test_profiler.get_thread_traceback_size()  );
   // NOTE: .size() returns size_t so am I allowed to compare types int & size_t like this?

}

TEST(HashTableTest,UpdateTest) { 

   // Create new hash 
   auto prof_main = test_profiler.hashtable_query_insert("Pie");

   // Initial time for this hash should be zero
   double t1 = test_profiler.get_thread0_walltime(prof_main);
   EXPECT_DOUBLE_EQ(t1, 0.0);

   // Start timing 
   test_profiler.start("Pie"); 

   sleep(1);

   double t2 = test_profiler.get_thread0_walltime(prof_main);

   sleep(1);
   
   // Stop timing  
   test_profiler.stop(prof_main);

   // End time
   double t3 = test_profiler.get_thread0_walltime(prof_main);

   // Ideally t3 > t2 > t1
   // EXPECT_GT(t3, t2); 
   // EXPECT_GT(t2, t1);
   // Instead, t1 = t2 
   EXPECT_GT(t3, t2);
   EXPECT_EQ(t2, t1);

}

/*TEST(HashTableTest,TableTest) { 

   // Various random tests related to the std::unordered_map table_ 
 
}*/

TEST(HashTableTest,WriteTest) {

   // Start 3 nested regions  
   auto prof_main = test_profiler.start("Shortbread"); 
   auto prof_sub = test_profiler.start("Brownie"); 
   auto prof_subsub = test_profiler.start("RockyRoad"); 

   // Stop them 1 by 1
   sleep(1);
   test_profiler.stop(prof_subsub); 
   sleep(1);
   test_profiler.stop(prof_sub);
   sleep(1);  
   test_profiler.stop(prof_main);

   // hashvec is ordered from high to low so... [lastEntry] < [firstEntry] 
   double val1 = test_profiler.get_hashvec().front().second.self_walltime_;
   double val_second_last = test_profiler.get_hashvec()[test_profiler.get_hashvec().size() - 2].second.self_walltime_;
   double val_last = test_profiler.get_hashvec().back().second.self_walltime_;
   
   EXPECT_LT(val_last, val_second_last);
   EXPECT_LT(val_second_last, val1);

   // Since hashvec is a vector of (hash,HashEntry) pairs 
   // it should be equal in size to the number of regions 
   EXPECT_EQ(test_profiler.get_hashvec().size(), 3);

}

TEST(ProfilerTest,WrongHashTest) {

   EXPECT_EXIT({

      // Start main
      auto prof_main = test_profiler.start("Chocolate");

      // First subregion
      auto prof_sub = test_profiler.start("Vanilla");
      test_profiler.stop(prof_sub); 

      // Another subregion, but with wrong hash in profiler.stop()
      auto prof_sub_sub = test_profiler.start("Strawberry");
      test_profiler.stop(prof_sub); 

      // Stop main
      test_profiler.stop(prof_main);

   }, testing::ExitedWithCode(100), "EMERGENCY STOP: hashes don't match."); 

}

     





