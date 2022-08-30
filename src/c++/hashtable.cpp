/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "hashtable.h"
#include <iomanip>
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

/**
 * @brief  Constructs a new entry in the hash table. 
 *
 */

HashEntry::HashEntry(std::string_view region_name)
      : region_name_(region_name)
      , total_walltime_(0.0)
      , total_raw_walltime_(0.0)
      , self_walltime_(0.0)
      , child_walltime_(0.0)
      , overhead_time_(0.0)
      {}

/**
 * @brief Hashtable constructor
 * 
 */

HashTable::HashTable(int const tid)
  : tid_(tid)
  {}

/**
 * @brief  Inserts a new entry into the hashtable.
 *
 */

size_t HashTable::query_insert(std::string_view region_name) noexcept
{
  size_t hash = hash_function_(region_name);

  if (table_.count(hash) == 0){
    table_.emplace(hash,HashEntry(region_name));
    assert (table_.count(hash) > 0);
  }

  return hash;
}

/**
 * @brief  Inserts a special entry. Such entries have the same hash across all
 *         threads.
 * @param [in]  region_name  The name of the special region.
 * @note   Since a special entry is created by the profiler code itself, it
 *         would be a coding error if there was an attempt to include the same
 *         key twice. Hence an assertion is used to catch this case.
 */

size_t HashTable::insert_special(std::string_view region_name)
{
  size_t hash = hash_function_(region_name);

  assert (table_.count(hash) == 0);
  table_.emplace(hash, HashEntry(region_name));
  assert (table_.count(hash) > 0);

  return hash;
}

/**
 * @brief  Updates the total walltime for the specified region. 
 * @param [in] hash  The hash corresponding to the profiled region.
 * @param [in] time_delta  The time increment to add.
 * @param [in] overhead_time_delta  The overhead time increment to add.
 */

void HashTable::update(size_t hash, double time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the number of calls and the walltime for this hash entry.
  auto& entry = table_.at(hash);
  entry.total_walltime_ += time_delta;
}

/**
 * @brief  Add child region time to parent, and also any profiling overhead time
 *         that must be subtracted from the parent's total time later.
 * @param [in] hash        The hash of the child region to update.
 * @param [in] time_delta  The time spent in the child region.
 * @param [in] overhead_time_delta  The time spent in profiler callipers when monitoring
 *                                  the child region.
 */

double* HashTable::add_child_time(size_t hash, double time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.child_walltime_ += time_delta;
  return &entry.overhead_time_; 
}

/**
 * @brief  Writes all entries in the hashtable, sorted according to self times.
 *
 */

void HashTable::write()
{

  std::string routine_at_thread = "Thread: " + std::to_string(tid_);

  // Write headings
  std::cout << "\n";
  std::cout
    << std::setw(40) << std::left  << routine_at_thread  << " "
    << std::setw(15) << std::right << "Self (s)"         << " "
    << std::setw(15) << std::right << "Total (raw) (s)"  << " "
    << std::setw(15) << std::right << "Total (s)"        << "\n";
 
  std::cout << std::setfill('-');
  std::cout
    << std::setw(40) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(15) << "-" << "\n";
  std::cout << std::setfill(' ');

  // Create a vector from the hashtable and sort the entries according to self
  // walltime.  If optimisation of this is needed, it ought to be possible to
  // acquire a vector of hash-selftime pairs in the correct order, then use the
  // hashes to look up other information directly from the hashtable.
  auto hashvec = std::vector<std::pair<size_t, HashEntry>>(begin(table_), end(table_));
  std::sort(begin(hashvec), end(hashvec), 
      [](auto a, auto b) { return a.second.self_walltime_ > b.second.self_walltime_;});
    
  // Data entries
  for (auto& [hash, entry] : hashvec) {
    std::cout 
      << std::setw(40) << std::left  << entry.region_name_         << " "
      << std::setw(15) << std::right << entry.self_walltime_       << " "
      << std::setw(15) << std::right << entry.total_raw_walltime_  << " "
      << std::setw(15) << std::right << entry.total_walltime_      << "\n";
  }
}

/**
 * @brief  Computes self times from total times and profiling overheads.
 *
 */

 void HashTable::prepare_computed_times(size_t const profiler_hash)
 {

   double total_overhead_time = 0.0;

   // Loop over entries in the hashtable. Although the hashtable does already
   // contain an entry for the profiler itself, the constructor will have
   // ensured that all numerical data members are zero.
   for (auto& [hash, entry] : table_) {
     entry.self_walltime_ = entry.total_walltime_   
                          - entry.child_walltime_ 
                          - entry.overhead_time_;
     entry.total_raw_walltime_ = entry.total_walltime_
                               - entry.overhead_time_;
     total_overhead_time += entry.overhead_time_;
   }

   // Set values for the profiler entry in the hashtable.
   table_.at(profiler_hash).self_walltime_      = total_overhead_time;
   table_.at(profiler_hash).total_walltime_     = total_overhead_time;
   table_.at(profiler_hash).total_raw_walltime_ = total_overhead_time;

 }

/**
 * @brief  Produce a list of keys stored in the hashtable.
 *
 */

std::vector<size_t> HashTable::list_keys()
{
  std::vector<size_t> keys;
  for (auto const& key : table_)
  {
    keys.push_back(key.first);
  }
  return keys;
}

/**
 * @brief  Get the total (inclusive) time corresponding to the input hash.
 * 
 */

double HashTable::get_total_walltime(size_t const hash)
{
    return table_.at(hash).total_walltime_;
}


