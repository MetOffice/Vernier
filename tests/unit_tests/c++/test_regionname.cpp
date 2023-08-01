/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include <chrono>

#include "vernier.h"

//
//  Tests focused on the "region name" of a particular section.
//  All main and sub-regions should give the expected string. Various
//  other funky region names will potentially be tested in the future.
//

TEST(RegionNameTest,NamesMatchTest) {

  //Start main region with name "Cappucino"
  const auto& prof_cappucino = meto::vernier.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto& prof_latte = meto::vernier.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName = meto::vernier.get_region_name(prof_latte,0);
    EXPECT_EQ("Latte@0", subregionName);

    meto::vernier.stop(prof_latte);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName = meto::vernier.get_region_name(prof_cappucino,0);
    EXPECT_EQ("Cappucino@0", regionName);
  }

  {
    SCOPED_TRACE("Problem with the profiler region name");

    // Get profiler region name out from the profiler and test
    auto const prof_self_handle = std::hash<std::string_view>{}("__vernier__@0");
    std::string profilerRegionName = meto::vernier.get_region_name(prof_self_handle,0);
    EXPECT_EQ("__vernier__@0", profilerRegionName);
  }

  meto::vernier.stop(prof_cappucino);

}

/**
 *  @TODO  Think about whether or not we want region name checks within code, i.e
 *         rules for what is and isn't allowed. See Issue #54.
 *
 */
/* Placeholder for testing different region names, which there are no checks for within the code yet...

TEST(RegionNameTest,InvalidsTest) {

  // Long name
  const auto& A = vernier.hashtable_query_insert("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

  // Nothing / blank space

  // Non-null-terminated string

  // Special characters

  // Initalise inside brackets

}*/
