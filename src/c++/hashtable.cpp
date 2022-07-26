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

/**
 * @brief  Constructs a new entry in the hash table. 
 *
 */

HashEntry::HashEntry(std::string_view region_name)
      : region_name_(region_name)
      , total_walltime_(0.0)
      , self_walltime_(0.0)
      , child_walltime_(0.0)
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
 * @brief  Updates entries in the hashtable.
 *
 */

void HashTable::update(size_t hash, double time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.total_walltime_ += time_delta;

}

/**
 * @brief  Add child time to parent, and recompute the self time.
 *
 */

void HashTable::add_child_time(size_t hash, double time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.child_walltime_ += time_delta;
}

/**
 * @brief  Writes all entries in the hashtable.
 *
 */

void HashTable::write()
{

  this->compute_self_times();

  std::string routine_at_thread = "Thread: " + std::to_string(tid_);

  // Write headings
  std::cout << "\n";
  std::cout
    << std::setw(40) << std::left  << routine_at_thread  << " "
    << std::setw(15) << std::right << "Self (s)"         << " "
    << std::setw(15) << std::right << "Total (s)"        << "\n";
 
  std::cout << std::setfill('-');
  std::cout
    << std::setw(40) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(15) << "-" << "\n";
  std::cout << std::setfill(' ');
    
  // Data entries
  for (auto& [hash, entry] : table_) {
    std::cout 
      << std::setw(40) << std::left  << entry.region_name_    << " "
      << std::setw(15) << std::right << entry.self_walltime_  << " "
      << std::setw(15) << std::right << entry.total_walltime_ << "\n";
  }
}

/**
 * @brief  Computes self times from total times.
 *
 */

 void HashTable::compute_self_times()
 {
   for (auto& [hash, entry] : table_) {
     entry.self_walltime_ = entry.total_walltime_ - entry.child_walltime_;
   }
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
 * @brief  Get the total wallclock time, which is the total walltime of the
 *         first entry in the table corresponding to the top-level timing
 *         callipers.
 *
 */

double HashTable::get_total_wallclock_time()
{
    return table_.begin()->second.total_walltime_;
}

double HashTable::get_self_wallclock_time()
{
    this->compute_self_times();
    return table_.begin()->second.self_walltime_;
}

double HashTable::get_child_wallclock_time()
{
    return table_.begin()->second.child_walltime_;
}

std::string HashTable::get_region_name() 
{
    return table_.begin()->second.region_name_; 
}

