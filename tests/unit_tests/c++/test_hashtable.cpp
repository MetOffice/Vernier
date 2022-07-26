#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

#include <gmock/gmock.h>
using ::testing::An;

static Profiler test_profiler;

/*TEST(HashTableTest,QueryInsertTest) { 

   // Start profiler timing 
   auto prof_main = test_profiler.start("SalmonQuiche"); 

   // Time something
   sleep(1);
   
   // Stop timing  
   test_profiler.stop(prof_main);  

   // 
   auto val1 = test_profiler.thread_hashtables_.query_insert("SalmonQuiche"); 
   
   EXPECT_THAT(val1, An<size_t>());
   EXPECT_EQ(test_profiler.thread_hashtables_.count(val1),1);

}*/ 

TEST(HashTableTest,ThreadsEqualsEntries) { 

   // Start profiler timing 
   auto prof_main = test_profiler.start("MAIN"); 

   // Time something
   sleep(1);
   
   // Stop timing  
   test_profiler.stop(prof_main); 

   // Compare number of threads and number of hashtable entries 
   EXPECT_EQ( test_profiler.max_threads_ , test_profiler.thread_hashtables_.size() ); 
   EXPECT_EQ( test_profiler.max_threads_ , test_profiler.thread_traceback_.size()  );
   // NOTE: .size() returns size_t so am I allowed to compare types int & size_t like this?
}


