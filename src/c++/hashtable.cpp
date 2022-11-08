/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashvec.h"
#include <cassert>
#include <string>


/**
 * @brief  Constructs a new entry in the hash table.
 *
 */

HashEntry::HashEntry(std::string_view region_name)
      : region_name_(region_name)
      , total_walltime_(time_duration_t::zero())
      , self_walltime_(time_duration_t::zero())
      , child_walltime_(time_duration_t::zero())
      , call_count_(0)
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

void HashTable::update(size_t hash, time_duration_t time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.total_walltime_ += time_delta;

  // Update the number of times this region has been called
  entry.call_count_++;
}

/**
 * @brief  Add child time to parent, and recompute the self time.
 *
 */

void HashTable::add_child_time(size_t hash, time_duration_t time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.child_walltime_ += time_delta;
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
 * @brief  Appends table_ onto the end of an input HashVec 
 * 
 */

void HashTable::append_to(std::vector<std::pair<size_t,HashEntry>>* hashvec_ptr)
{
  compute_self_times();
  hashvec_ptr->insert(hashvec_ptr->end(), table_.begin(), table_.end());
}

/**
 * @brief  Get the total (inclusive) time corresponding to the input hash.
 *
 */

double HashTable::get_total_walltime(size_t const hash) const
{
  return table_.at(hash).total_walltime_.count();
}

/**
 * @brief  Get the profiler self time corresponding to the input hash.
 *
 */

double HashTable::get_self_walltime(size_t const hash)
{
  this->compute_self_times();
  return table_.at(hash).self_walltime_.count();
}

/**
 * @brief  Get the child time corresponding to the input hash.
 *
 */

double HashTable::get_child_walltime(size_t const hash) const
{
  return table_.at(hash).child_walltime_.count();
}

/**
 * @brief  Get the region name corresponding to the input hash.
 *
 */

std::string HashTable::get_region_name(size_t const hash) const
{
  return table_.at(hash).region_name_;
}

 /**
  * @brief  Get the number of times the input hash region has been called.
  *
  * @param[in] hash  The hash corresponding to the region of interest.
  *
  * @returns  Returns an integer corresponding to the number of the times the
  *           region of interest has been called within the code being profiled.
  *
  */

unsigned long long int HashTable::get_call_count(size_t const hash) const
{
    return table_.at(hash).call_count_;
}
