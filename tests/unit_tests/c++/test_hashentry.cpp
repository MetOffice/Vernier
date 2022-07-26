#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

static Profiler test_profiler;

TEST(HashTableTest, HashEntriesTest) {

   // Instantiate a profiler with region name of "QuicheLorraine"
   auto prof_main = test_profiler.start("QuicheLorraine");
   sleep(1);

   /* // Time a region 
   { 
    auto prof_sub = test_profiler.start("SUB"); 
    sleep(1); 
    
    // Get subregion name out from profiler and test
    const char* subregionName = test_profiler.get_region_name().c_str(git ); 
    // EXPECT_STREQ("SUB",subregionName);

    test_profiler.stop(prof_sub);
   } */

   // Get region name out from profiler and test
   const char* regionName = test_profiler.get_region_name().c_str(); 
   EXPECT_STREQ("QuicheLorraine",regionName);

   // Stop timing  
   test_profiler.stop(prof_main); 

   // Grab the total, child and self wallclock times
   double total = test_profiler.get_total_wallclock_time(); 
   double child = test_profiler.get_child_wallclock_time();
   double self = test_profiler.get_self_wallclock_time();

   // Test that self_walltime = total_walltime - child_walltime
   EXPECT_EQ(self,total-child);

}
