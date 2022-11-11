/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

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
      , total_walltime_      (time_duration_t::zero())
      , total_raw_walltime_  (time_duration_t::zero())
      , self_walltime_       (time_duration_t::zero())
      , child_walltime_      (time_duration_t::zero())
      , overhead_walltime_   (time_duration_t::zero())
      , call_count_(0)
      {}

/**
 * @brief Hashtable constructor
 * @param [in] tid  The thread ID.
 *
 */

HashTable::HashTable(int const tid)
  : tid_(tid)
{

  // Set the name and hash of the profiler entry.
  std::string const profiler_name = "__profiler__";
  profiler_hash_ = hash_function_(profiler_name);

  // Insert special entry for the profiler overhead time.
  assert (table_.count(profiler_hash_) == 0);
  table_.emplace(profiler_hash_, HashEntry(profiler_name));
  assert (table_.count(profiler_hash_) > 0);
}

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
 * @brief  Updates the total walltime and call count for the specified region. 
 * @param [in] hash  The hash corresponding to the profiled region.
 * @param [in] time_delta  The time increment to add.
 */

void HashTable::update(size_t const hash, time_duration_t const time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry.
  auto& entry = table_.at(hash);
  entry.total_walltime_ += time_delta;

  // Update the number of times this region has been called
  ++entry.call_count_;

}

/**
 * @brief
 *
 */

time_duration_t* HashTable::add_child_time(
                         size_t const hash,
                         time_duration_t child_walltime)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  auto& entry = table_.at(hash);
  entry.child_walltime_ += child_walltime;
  return &entry.overhead_walltime_;
}

/**
 * @brief
 *
 */

time_duration_t& HashTable::increment_profiler_calls()
{
  auto& entry = table_.at(profiler_hash_);
  ++entry.call_count_;
  return entry.total_walltime_;
}

/**
 * @brief  Add child region and overhead times to parent.
 * @param [in] hash        The hash of the child region to update.
 * @param [in] time_delta  The time spent in the child region.
 */

/**
 * @brief  Writes all entries in the hashtable, sorted according to self times.
 *
 */

void HashTable::write()
{

  // Ensure all computed times are up-to-date.
  prepare_computed_times_all();

  std::string routine_at_thread = "Thread: " + std::to_string(tid_);

  // Write headings
  std::cout << "\n";
  std::cout
    << std::setw(40) << std::left  << routine_at_thread  << " "
    << std::setw(15) << std::right << "Self (s)"         << " "
    << std::setw(15) << std::right << "Total (raw) (s)"  << " "
    << std::setw(15) << std::right << "Total (s)"        << " "
    << std::setw(10) << std::right << "Calls"            << "\n";

  std::cout << std::setfill('-');
  std::cout
    << std::setw(40) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(15) << "-" << " "
    << std::setw(10) << "-" << "\n";
  std::cout << std::setfill(' ');

  // Create a vector from the hashtable and sort the entries according to self
  // walltime.  If optimisation of this is needed, it ought to be possible to
  // acquire a vector of hash-selftime pairs in the correct order, then use the
  // hashes to look up other information directly from the hashtable.
  hashvec = std::vector<std::pair<size_t, HashEntry>>(table_.cbegin(), table_.cend());
  std::sort(begin(hashvec), end(hashvec),
      [](auto a, auto b) { return a.second.self_walltime_ > b.second.self_walltime_;});

  // Data entries
  for (auto& [hash, entry] : hashvec) {
    std::cout
      << std::setw(40) << std::left  << entry.region_name_                << " "
      << std::setw(15) << std::right << entry.self_walltime_.count()      << " "
      << std::setw(15) << std::right << entry.total_raw_walltime_.count() << " "
      << std::setw(15) << std::right << entry.total_walltime_.count()     << " "
      << std::setw(10) << std::right << entry.call_count_                 << "\n";
  }
}

/**
 * @brief  Evaluates times derived from other times measured, for a particular
 *         code region.
 * @detail Times computed are: the region self time and the total time minus
 *         directly incurred profiling overhead costs.
 *
 * @param [in] hash   The hash of the region to compute.
 */

void HashTable::prepare_computed_times(size_t const hash)
{
  auto& entry = table_.at(hash);

  // Self time
  entry.self_walltime_ = entry.total_walltime_
                       - entry.child_walltime_
                       - entry.overhead_walltime_;

  // Total walltime with overheads attributed to the parent removed.
  entry.total_raw_walltime_ = entry.total_walltime_
                            - entry.overhead_walltime_;
}

/**
 * @brief  Evaluates times derived from other times measured, looping over all
 *         code regions; includes updating the special profiling overhead entry.
 */

 void HashTable::prepare_computed_times_all()
 {

   auto total_overhead_time = time_duration_t::zero();

   // Loop over entries in the hashtable. This would include the special
   // profiler entry, but the HashEntry constructor will have ensured that all
   // corresponding values are zero thus far.
   for (auto& [hash, entry] : table_) {
     prepare_computed_times(hash);
   }

   /// // Check that the special profiler hash entries are all zero, even after the
   /// // above loop.
   /// assert(table_.at(profiler_hash_).self_walltime_      == time_duration_t::zero());
   /// assert(table_.at(profiler_hash_).child_walltime_     == time_duration_t::zero());
   /// assert(table_.at(profiler_hash_).total_walltime_     == time_duration_t::zero());
   /// assert(table_.at(profiler_hash_).total_raw_walltime_ == time_duration_t::zero());

   // Set values for the profiler entry specifically in the hashtable.
   // table_.at(profiler_hash_).self_walltime_      = total_overhead_time_;
   // table_.at(profiler_hash_).child_walltime_     = time_duration_t::zero();
   // table_.at(profiler_hash_).total_walltime_     = total_overhead_time_;
   // table_.at(profiler_hash_).total_raw_walltime_ = total_overhead_time_;

   // std::cout << "My    thread ID: " << omp_get_thread_num() << std::endl;
   // std::cout << "Table thread ID: " << tid_                 << std::endl;

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
 * @brief  Get the total (inclusive) time of the specified region.
 * @param [in]  The hash corresponding to the region.
 */

double HashTable::get_total_walltime(size_t const hash) const
{
  return table_.at(hash).total_walltime_.count();
}

/**
 * @brief  Get the total time of the specified region, minus profiling overheads
 *         incurred by calling direct children.
 * @param [in]  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_total_raw_walltime(size_t const hash)
{
   prepare_computed_times(hash);
   return table_.at(hash).total_raw_walltime_.count();
}

/**
 * @brief  Get the profiling overhead time for a specified region, as incurred
 *         by calling direct children. 
 * @param [in]  The hash corresponding to the region.
 */

double HashTable::get_overhead_walltime(size_t const hash) const
{
  return table_.at(hash).overhead_walltime_.count();
}

/**
 * @brief  Get the profiler self (exclusive) time corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_self_walltime(size_t const hash)
{
  prepare_computed_times(hash);
  return table_.at(hash).self_walltime_.count();
}

/**
 * @brief  Get the child time corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_child_walltime(size_t const hash) const
{
  return table_.at(hash).child_walltime_.count();
}

/**
 * @brief  Get the region name corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
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

/**
 * @brief  Get the number of calliper pairs called.
 *
 * @returns  Returns an integer corresponding to the number of the times pairs
 *           of callipers have been called. 
 *           
 */

unsigned long long int HashTable::get_prof_call_count() const
{
    return table_.at(profiler_hash_).call_count_;
}

