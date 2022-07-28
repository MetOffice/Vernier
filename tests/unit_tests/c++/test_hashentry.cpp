#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

static Profiler test_profiler;

TEST(HashTableTest, HashEntriesTest) {

   // Instantiate a profiler with region name of "QuicheLorraine"
   auto prof_main = test_profiler.start("QuicheLorraine");
   sleep(1);

   // Time a sub-region 
   { 
    auto prof_sub = test_profiler.start("SalmonQuiche"); 
    sleep(1); 
    
    // Get subregion name out from profiler and test
    const char* subregionName = test_profiler.get_thread0_region_name(prof_sub).c_str(); 
    EXPECT_STREQ("SalmonQuiche",subregionName);

    test_profiler.stop(prof_sub);
   } 

   // Get region name out from profiler and test
   const char* regionName = test_profiler.get_thread0_region_name(prof_main).c_str(); 
   EXPECT_STREQ("QuicheLorraine",regionName);

   // Stop timing  
   test_profiler.stop(prof_main); 

   // Grab the total, child and self wallclock times
   double total = test_profiler.get_thread0_walltime(prof_main); 
   double child = test_profiler.get_thread0_child_walltime(prof_main);
   double self  = test_profiler.get_thread0_self_walltime(prof_main);

   // Test that self_walltime = total_walltime - child_walltime
   EXPECT_EQ(self,total-child);

}
