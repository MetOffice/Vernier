/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "vernier.h"

//
//  Tests focused on the "region name" of a particular section.
//  All main and sub-regions should give the expected string. Various
//  other funky region names will potentially be tested in the future.
//

int const tid = 0;
std::string tid_str(std::to_string(tid));
std::string tid_bytes(reinterpret_cast<char const *>(&tid), sizeof(tid));

TEST(RegionNameTest, NamesMatchTest) {

  meto::vernier.init();

  // Start main region with name "Cappucino"
  const auto &prof_cappucino = meto::vernier.start("Cappucino");

  {
    SCOPED_TRACE("Problem with sub-region name");

    // Start timing a sub-region with name "Latte"
    std::string myString = "Latte";
    const auto &prof_latte = meto::vernier.start(myString);

    // Get subregion name out from profiler and check it is what we expect
    std::string subregionName =
        meto::vernier.get_decorated_region_name(prof_latte, tid);
    EXPECT_EQ("Latte@" + tid_str, subregionName);

    meto::vernier.stop(prof_latte);
  }

  {
    SCOPED_TRACE("Problem with main region name");

    // Get main region name out from profiler and test
    std::string regionName =
        meto::vernier.get_decorated_region_name(prof_cappucino, tid);
    EXPECT_EQ("Cappucino@" + tid_str, regionName);
  }

  {
    SCOPED_TRACE("Problem with the profiler region name");

    // Get profiler region name out from the profiler and test
    auto const prof_self_handle =
        std::hash<std::string_view>{}("__vernier__" + tid_bytes);
    std::string profilerRegionName =
        meto::vernier.get_decorated_region_name(prof_self_handle, tid);
    EXPECT_EQ("__vernier__@" + tid_str, profilerRegionName);
  }

  meto::vernier.stop(prof_cappucino);

  meto::vernier.finalize();
}

/**
 *  @TODO  Think about whether or not we want region name checks within code,
 * i.e rules for what is and isn't allowed. See Issue #54.
 *
 */
/* Placeholder for testing different region names, which there are no checks for
within the code yet...

TEST(RegionNameTest,InvalidsTest) {

  // Long name
  const auto& A =
vernier.hashtable_query_insert("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

  // Nothing / blank space

  // Non-null-terminated string

  // Special characters

  // Initalise inside brackets

}*/
