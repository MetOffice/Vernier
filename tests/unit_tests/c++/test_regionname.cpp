/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <profiler.h>
#include <gtest/gtest.h>
#include <string>

#include "profiler.h"

//
//  Tests focused on the "region name" of a particular section.
//  All main and sub-regions should give the expected string. Various
//  other funky region names will potentially be tested in the future.
//

int const tid = 0;
std::string tid_str(std::to_string(tid));
std::string tid_bytes(reinterpret_cast<char const*>(&tid), sizeof(tid));

TEST(RegionNameTest,NamesMatchTest) {

  //Start main region with name "Cappucino"
  const auto& prof_cappucino = prof.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto& prof_latte = prof.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName = prof.get_decorated_region_name(prof_latte, tid);
    EXPECT_EQ("Latte@" + tid_str, subregionName);

    prof.stop(prof_latte);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName = prof.get_decorated_region_name(prof_cappucino, tid);
    EXPECT_EQ("Cappucino@" + tid_str, regionName);
  }

  {
    SCOPED_TRACE("Problem with the profiler region name");

    // Get profiler region name out from the profiler and test
    auto const prof_self_handle = std::hash<std::string_view>{}("__profiler__@" + tid_bytes);
    std::string profilerRegionName = prof.get_decorated_region_name(prof_self_handle, tid);
    EXPECT_EQ("__profiler__@" + tid_str, profilerRegionName);
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
