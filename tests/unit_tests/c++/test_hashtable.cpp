#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

static Profiler test_profiler;

TEST(HashTableTest,ThreadsEqualsEntries) { 

   // Start profiler timing 
   auto prof_main = test_profiler.start("MAIN"); 

   // Time something
   sleep(1);
   
   // Stop timing  
   test_profiler.stop(prof_main); 

   // Compare number of threads and number of hashtable entries 
   EXPECT_EQ( test_profiler.max_threads_ , test_profiler.thread_hashtables_.size() );
   // NOTE: .size() returns size_t so am I allowed to compare types int & size_t like this?
}


