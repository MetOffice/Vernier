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
 *  struct (RegionRecord) collects together information pertinent to a single
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

struct NullHashFunction {
  std::size_t operator()(std::size_t const& key) const {
      return key;
  }
};

/**
 * @brief  Structure to hold information for a particular routine.
 *
 * Bundles together any information pertinent to a specific profiled region.
 *
 */

struct RegionRecord{
  public:

    // Constructor
    RegionRecord() = delete;
    explicit RegionRecord(size_t const, std::string_view);

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

typedef std::vector<RegionRecord>::size_type  record_iterator_t;

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
    record_iterator_t profiler_iterator_;
    
    // Hash function
    std::hash<std::string_view> hash_function_;
    
    // Hashtable containing locations of region records. 
    std::unordered_map<size_t, record_iterator_t, NullHashFunction> lookup_table_;

    // Vector of region records.
    std::vector<RegionRecord> hashvec_;

    // Private member functions
    void prepare_computed_times(RegionRecord&);
    void prepare_computed_times_all();

  public:

    // Constructors
    HashTable() = delete;
    HashTable(int);

    // Prototypes
    void query_insert(std::string_view, size_t&, record_iterator_t&) noexcept;
    void update(record_iterator_t const, time_duration_t const);
    void write();

    // Member functions
    std::vector<size_t> list_keys();
    time_duration_t* add_child_time(record_iterator_t const, time_duration_t);
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
    record_iterator_t      hash2iterator(size_t const);
    record_iterator_t      hash2iterator_const(size_t const) const;
};
#endif

