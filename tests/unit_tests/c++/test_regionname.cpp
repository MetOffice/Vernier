#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>


//
//  Tests focused on the "region name" of a particular section.
//  All main and sub-regions should give the expected string. Various
//  other funky region names will potentially be tested in the future.
//

TEST(RegionNameTest,NamesMatchTest) {

  //Start main region with name "Cappucino"
  const auto& prof_main = prof.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto& prof_sub = prof.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName = prof.get_thread0_region_name(prof_sub);
    EXPECT_EQ("Latte", subregionName);

    prof.stop(prof_sub);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName = prof.get_thread0_region_name(prof_main);
    EXPECT_EQ("Cappucino", regionName);
  }

  prof.stop(prof_main);

}

/* Placeholder for testing different region names, which there are no checks for within the code yet...

TEST(RegionNameTest,InvalidsTest) {

  // Long name
  const auto& A = prof.hashtable_query_insert("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

  // Nothing / blank space

  // Non-null-terminated string

  // Special characters

  // Initalise inside brackets

}*/
