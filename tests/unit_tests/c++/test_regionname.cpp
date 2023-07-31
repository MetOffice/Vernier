/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <chrono>

#include "profiler.h"

//
//  Tests focused on the "region name" of a particular section.
//  All main and sub-regions should give the expected string. Various
//  other funky region names will potentially be tested in the future.
//

TEST(RegionNameTest,NamesMatchTest) {

  //Start main region with name "Cappucino"
  const auto& prof_cappucino = meto::prof.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto& prof_latte = meto::prof.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName = meto::prof.get_region_name(prof_latte,0);
    EXPECT_EQ("Latte@0", subregionName);

    meto::prof.stop(prof_latte);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName = meto::prof.get_region_name(prof_cappucino,0);
    EXPECT_EQ("Cappucino@0", regionName);
  }

  {
    SCOPED_TRACE("Problem with the profiler region name");

    // Get profiler region name out from the profiler and test
    auto const prof_self_handle = std::hash<std::string_view>{}("__profiler__@0");
    std::string profilerRegionName = meto::prof.get_region_name(prof_self_handle,0);
    EXPECT_EQ("__profiler__@0", profilerRegionName);
  }

  meto::prof.stop(prof_cappucino);

}

/**
 *  @TODO  Think about whether or not we want region name checks within code, i.e
 *         rules for what is and isn't allowed. See Issue #54.
 *
 */
/* Placeholder for testing different region names, which there are no checks for within the code yet...

TEST(RegionNameTest,InvalidsTest) {

  // Long name
  const auto& A = meto::prof.hashtable_query_insert("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

  // Nothing / blank space

  // Non-null-terminated string

  // Special characters

  // Initalise inside brackets

}*/
