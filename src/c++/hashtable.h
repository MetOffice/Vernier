/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

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
#include <fstream>

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
    std::string region_name_;
    double      total_walltime_;
    double      self_walltime_;
    double      child_walltime_;

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
    std::unordered_map<size_t,HashEntry> table_;
    std::hash<std::string_view> hash_function_;

  public:

    // Constructors
    HashTable() = delete;
    HashTable(int);

    // Prototypes
    size_t query_insert(std::string_view) noexcept;
    void update(size_t, double);
    void write(std::ofstream&);

    // Member functions
    std::vector<size_t> list_keys();
    void add_child_time(size_t, double);
    void compute_self_times();
    double get_total_walltime(size_t const);

};
#endif
