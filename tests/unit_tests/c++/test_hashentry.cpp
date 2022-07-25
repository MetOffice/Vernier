#include <iostream>
#include <gtest/gtest.h> 
#include <profiler.h>

static Profiler test_profiler;

TEST(HashEntryTest, WalltimesTest) {

   // Start profiler timing 
   auto prof_main = test_profiler.start("MAIN"); 
   sleep(1);

   // Time a region 
   { 
    auto prof_sub = test_profiler.start("SUB"); 
    sleep(1); 
    test_profiler.stop(prof_sub); 
   }
   
   // Stop timing 
   sleep(1); 
   test_profiler.stop(prof_main); 

   // Grab the total, child and self wallclock times
   double total = test_profiler.get_total_wallclock_time(); 
   double child = test_profiler.get_child_wallclock_time();
   double self = test_profiler.get_self_wallclock_time();
 
   // Test that self_walltime = total_walltime - child_walltime
   EXPECT_EQ(self,total-child) << "Fault in self wallclock time calculation";
}

// Test region_name_ somehow?
/*TEST(HashEntryTest, RegionNameTest) { 

   auto prof_main_prof_main_prof_main_prof_main) = test_profiler.start("MainSectionAndMakingThisStringVeryLongOnPurposeToSeeWhatHappens");
   sleep(1);
   test_profiler.stop(prof_main_prof_main_prof_main_prof_main);
  
   SUCCEED(); 
}*/ 
 

