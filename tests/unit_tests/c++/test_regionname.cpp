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
  const auto& prof_cappucino = prof.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto& prof_latte = prof.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName = prof.get_region_name(prof_latte);
    EXPECT_EQ("Latte", subregionName);

    prof.stop(prof_latte);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName = prof.get_region_name(prof_cappucino);
    EXPECT_EQ("Cappucino", regionName);
  }

  prof.stop(prof_cappucino);

}

/**
 *  @TODO  Think about whether or not we want region name checks within code, i.e
 *         rules for what is and isn't allowed. See Issue #54.
 *
 */
/* Placeholder for testing different region names, which there are no checks for within the code yet...

TEST(RegionNameTest,InvalidsTest) {

  // Long name
  const auto& A = prof.hashtable_query_insert("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

  // Nothing / blank space

  // Non-null-terminated string

  // Special characters

  // Initalise inside brackets

}*/
