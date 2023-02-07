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

#ifndef PROFILER_HASHVEC_H
#define PROFILER_HASHVEC_H

#include <string>
#include <string_view>

#include "prof_gettime.h"

/**
 * @brief  Structure to hold information for a particular region.
 *
 */

struct HashEntry{
  public:

    // Constructor
    HashEntry() = delete;
    explicit HashEntry(std::string_view);

    // Data members
    std::string      region_name_;
    time_duration_t  total_walltime_;
    time_duration_t  total_raw_walltime_;
    time_duration_t  self_walltime_;
    time_duration_t  child_walltime_;
    time_duration_t  overhead_walltime_;
    unsigned long long int call_count_;

};

// Define the hashvec type.
using hashvec_t = std::vector<std::pair<size_t, HashEntry>>;

#endif

