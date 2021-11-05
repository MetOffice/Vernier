/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#ifndef HASHTABLE_H 
#define HASHTABLE_H

#include <iostream>
#include <map>
#include <list>
#include <utility>
#include <string>
#include <functional>
#include <cassert>
#include <vector>
#include <string_view>

/**
 * @brief  Structure to hold information for a particular routine.
 * */

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
 * @brief
 *
 * @description Interfaces with a C++ list to add entries to a linked list
 *              with some calculated hash.
 *
 */

class HashTable{
  
  private:

    // Members
    int tid_;
    std::map<size_t,HashEntry> table_;
    std::hash<std::string_view> hash_function_;

  public:

    // Constructors
    HashTable() = delete;
    HashTable(int);

    // Prototypes
    size_t query_insert(std::string_view) noexcept;
    void update(size_t, double);
    void write();

    // Member functions
    double get_walltime(size_t const hash) const {return table_.at(hash).total_walltime_;}

    std::vector<size_t> list_keys();

    void add_child_time(size_t, double);
    void compute_self_times();

};
#endif
