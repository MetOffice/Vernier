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
 * @brief  Writes all entries in the hashtable, sorted according to self times.
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
 * @brief  Get the total (inclusive) time corresponding to the input hash.
 * 
 */

double HashTable::get_total_walltime(size_t const hash)
{
  return table_.at(hash).total_walltime_;
}

/**
 * @brief  Get the profiler self time corresponding to the input hash.
 * 
 */

double HashTable::get_self_walltime(size_t const hash)
{
  this->compute_self_times();
  return table_.at(hash).self_walltime_;
}

/**
 * @brief  Get the child time corresponding to the input hash.
 * 
 */

double HashTable::get_child_walltime(size_t const hash)
{
  return table_.at(hash).child_walltime_;
}

/**
 * @brief  Get the region name corresponding to the input hash.
 * 
 */

std::string HashTable::get_region_name(size_t const hash) 
{
  return table_.at(hash).region_name_; 
}

/**
 * @brief  Get the hashtable .count() corresponding to the input hash.
 * 
 */

size_t HashTable::get_hashtable_count(size_t const hash) 
{
  return table_.count(hash);
}

/**
 * @brief  Returns TRUE if the hashtable is empty and false otherwise.
 * 
 */

bool HashTable::is_table_empty() 
{
  return table_.empty();
}

/**
 * @brief  Get the vector of pairs in profiler.write() used to sort entries from the underordered_map from high to low self walltime.
 * 
 */

std::vector<std::pair<size_t, HashEntry>> HashTable::get_hashvec() 
{
  this->write();
  return hashvec;
}
