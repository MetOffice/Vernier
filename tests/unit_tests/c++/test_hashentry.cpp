#include <iostream>
#include <profiler.h>

#include <gtest/gtest.h> 

TEST(HashTableTest, HashEntriesTest) {

   // Instantiate a profiler with region name of "QuicheLorraine"
   auto prof_main = prof.start("QuicheLorraine");
   sleep(1);

   // Time a sub-region 
   { 
    auto prof_sub = prof.start("SalmonQuiche"); 
    sleep(1); 
    
    // Get subregion name out from profiler and test
    const char* subregionName = prof.get_thread0_region_name(prof_sub).c_str(); 
    EXPECT_STREQ("SalmonQuiche",subregionName);

    prof.stop(prof_sub);
   } 

   // Get region name out from profiler and test
   const char* regionName = prof.get_thread0_region_name(prof_main).c_str(); 
   EXPECT_STREQ("QuicheLorraine",regionName);

   // Stop timing  
   prof.stop(prof_main); 

   // Grab the total, child and self wallclock times
   double total = prof.get_thread0_walltime(prof_main); 
   double child = prof.get_thread0_child_walltime(prof_main);
   double self  = prof.get_thread0_self_walltime(prof_main);

   // Test that self_walltime = total_walltime - child_walltime
   EXPECT_EQ(self,total-child);
   EXPECT_GT(total,child);

}
