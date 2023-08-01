/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   hashvec.h
 *  @brief  Defines struct and datatype for timed region entries in the
 *          hash vector.
 */

#ifndef VERNIER_HASHVEC_H
#define VERNIER_HASHVEC_H

#include <string>
#include <string_view>
#include <vector>

#include "vernier_gettime.h"

namespace meto
{

namespace meto
{

/**
 * @brief  Structure to hold information for a particular region.
 *
 * Bundles together any information pertinent to a specific profiled region.
 *
 */

struct RegionRecord {
  public:

    // Constructor
    RegionRecord() = delete;
    explicit RegionRecord(size_t const, std::string_view const);

    // Data members
    size_t           region_hash_;
    std::string      region_name_;
    time_duration_t  total_walltime_;
    time_duration_t  total_raw_walltime_;
    time_duration_t  self_walltime_;
    time_duration_t  child_walltime_;
    time_duration_t  overhead_walltime_;
    unsigned long long int call_count_;

};

// Define the hashvec type.
using hashvec_t = std::vector<RegionRecord>;

// Type definitions
using record_index_t = std::vector<RegionRecord>::size_type;

} // End meto namespace

#endif

