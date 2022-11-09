/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   hashtable.h
 *  @brief  Handles entries for each timed region.
 *
 *  In order to store region timings, one struct and one class are defined. The
 *  struct (HashEntry) collects together information pertinent to a single
 *  profiled region, such as its name, total time and self time.
 *
 *  The HashTable class contains a hashtable to hold the hash entries (see
 *  above). The hashing algorithm is bundled with it, so that it remains an
 *  internal implementation detail only.
 *
 *  An assortment of methods handle admin. tasks, such as adding new entries.
 *
 */

#ifndef PROFILER_HASHTABLE_H
#define PROFILER_HASHTABLE_H

#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <chrono>

#include "prof_gettime.h"


/**
 * @brief  Structure to hold information for a particular routine.
 *
 * Bundles together any information pertinent to a specific profiled region.
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

/**
 * @brief  Wraps STL hashtables with additional functionality.
 *
 * Bundles together an STL hashtable with the hashing algorithm, and adds
 * e.g. time-handling methods.
 *
 */

class HashTable{

  private:

    // Members
    int tid_;
    size_t profiler_hash_;
    std::unordered_map<size_t,HashEntry> table_;
    std::hash<std::string_view> hash_function_;
    std::vector<std::pair<size_t, HashEntry>> hashvec;

    // Private member functions
    void prepare_computed_times(size_t const);
    void prepare_computed_times_all();

  public:

    // Constructors
    HashTable() = delete;
    HashTable(int);

    // Prototypes
    size_t query_insert(std::string_view) noexcept;
    void update(size_t const, time_duration_t);
    void write();

    // Member functions
    std::vector<size_t> list_keys();
    time_duration_t* add_child_time(size_t const, time_duration_t);
    time_duration_t& increment_profiler_calls();

    // Getters
    double                 get_total_walltime(size_t const hash) const;
    double                 get_total_raw_walltime(size_t const hash);
    double                 get_overhead_walltime(size_t const hash) const;
    double                 get_self_walltime(size_t const hash);
    double                 get_child_walltime(size_t const hash) const;
    std::string            get_region_name(size_t const hash) const;
    unsigned long long int get_call_count(size_t const hash) const;
    unsigned long long int get_prof_call_count() const;
};
#endif

