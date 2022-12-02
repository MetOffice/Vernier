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

#define PROF_HASHVEC_RESERVE_SIZE 1000

/**
 * @brief  Constructs a new region record.
 * @param [in]  region_hash_  Hash of the region name.
 * @param [in]  region_name_  The region name.
 *
 */

RegionRecord::RegionRecord(size_t const region_hash, std::string_view const region_name)
      : region_hash_(region_hash)
      , region_name_(region_name)
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
  // Reserve enough places in hashvec_
  hashvec_.reserve(PROF_HASHVEC_RESERVE_SIZE);

  // Insert special entry for the profiler overhead time.
  query_insert("__profiler__", profiler_hash_, profiler_index_);
}

/**
 * @brief  Inserts a new entry into the hashtable.
 * @param [in]  region_name  The name of the region.
 * @param [out] hash          Hash of the region name.
 * @param [out] record_index  Array index of the region record.
 *
 */

void HashTable::query_insert(std::string_view const region_name, 
                             size_t& hash,
                             record_index_t& record_index) noexcept
{
  hash = hash_function_(region_name);

  // Found entry,
  if (auto search = lookup_table_.find(hash); search != lookup_table_.end())
  {
    record_index = search->second;
  }
  else
  {
    hashvec_.emplace_back(hash, region_name);
    record_index = hashvec_.size()-1;
    lookup_table_.emplace(hash, record_index);
    assert (lookup_table_.count(hash) > 0);
  }
}

/**
 * @brief  Updates the total walltime and call count for the specified region. 
 * @param [in] hash  The hash corresponding to the profiled region.
 * @param [in] time_delta  The time increment to add.
 */

void HashTable::update(record_index_t const record_index, time_duration_t const time_delta)
{

  auto& record = hashvec_[record_index];

  // Increment the walltime for this hash entry.
  record.total_walltime_ += time_delta;

  // Update the number of times this region has been called
  ++record.call_count_;

}

/**
 * @brief  Add child region and overhead times to parent.
 * @param [in] record_index  The index corresponding to the region record.
 * @param [in] time_delta    The time spent in the child region.
 * @returns  Pointer to the overhead time for this region. 
 */

time_duration_t* HashTable::add_child_time(
                         record_index_t  const record_index,
                         time_duration_t const child_walltime)
{
  auto& record = hashvec_[record_index];
  record.child_walltime_ += child_walltime;
  return &record.overhead_walltime_;
}

/**
 * @brief    Add child region and overhead times to parent.
 * @returns  Reference to the total profiling overhead time.
 */

time_duration_t& HashTable::increment_profiler_calls()
{
  auto& record = hashvec_[profiler_index_];
  ++record.call_count_;
  return record.total_walltime_;
}

/**
 * @brief  Sorts entries in the vector of region records according to self time
 *         and updates the hashtable with the new indices.
 */

void HashTable::sort_records()
{

  // Sort the entries according to self walltime.
  std::sort(begin(hashvec_), end(hashvec_),
      [](auto a, auto b) { return a.self_walltime_ > b.self_walltime_;});

  // Need to re-store the indices in the lookup table, since they will have all
  // moved around as a result of the above sort.
  for (auto it = begin(hashvec_); it != end(hashvec_); ++it) {
    auto current_index = it - hashvec_.begin();
    lookup_table_[it->region_hash_] =  static_cast<record_index_t>(current_index);
  }

}

/**
 * @brief  Writes all entries in the hashtable.
 * @note   Calls the method to sort times according to their region self-time.
 *
 */

void HashTable::write()
{

  // Ensure all computed times are up-to-date.
  prepare_computed_times_all();
  sort_records();

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

  // Data entries
  for (auto& record : hashvec_) {
    std::cout
      << std::setw(40) << std::left  << record.region_name_                << " "
      << std::setw(15) << std::right << record.self_walltime_.count()      << " "
      << std::setw(15) << std::right << record.total_raw_walltime_.count() << " "
      << std::setw(15) << std::right << record.total_walltime_.count()     << " "
      << std::setw(10) << std::right << record.call_count_                 << "\n";
  }
}

/**
 * @brief  Evaluates times derived from other times measured, for a particular
 *         code region.
 * @detail Times computed are: the region self time and the total time minus
 *         directly incurred profiling overhead costs.
 *
 * @param [in] record  The region record to compute.
 */

void HashTable::prepare_computed_times(RegionRecord& record)
{

  // Self time
  record.self_walltime_ = record.total_walltime_
                        - record.child_walltime_
                        - record.overhead_walltime_;

  // Total walltime with overheads attributed to the parent removed.
  record.total_raw_walltime_ = record.total_walltime_
                             - record.overhead_walltime_;
}

/**
 * @brief  Evaluates times derived from other times measured, looping over all
 *         code regions.
 */

void HashTable::prepare_computed_times_all()
{

  auto total_overhead_time = time_duration_t::zero();

  // Loop over entries in the hashtable.
  for (auto& [hash, index] : lookup_table_) {
    prepare_computed_times(hash2record(hash));
  }

}

/**
 * @brief  Produce a list of keys stored in the hashtable.
 *
 */

std::vector<size_t> HashTable::list_keys()
{
  std::vector<size_t> keys;
  for (auto const& key : lookup_table_)
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
  auto& record = hash2record_const(hash);

  return record.total_walltime_.count();
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
  auto& record = hash2record(hash);
   prepare_computed_times(record);
   return record.total_raw_walltime_.count();
}

/**
 * @brief  Get the profiling overhead time for a specified region, as incurred
 *         by calling direct children. 
 * @param [in]  The hash corresponding to the region.
 */

double HashTable::get_overhead_walltime(size_t const hash) const
{
  auto& record = hash2record_const(hash);
  return record.overhead_walltime_.count();
}

/**
 * @brief  Get the profiler self (exclusive) time corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_self_walltime(size_t const hash)
{
  auto& record = hash2record(hash);
  prepare_computed_times(record);
  return record.self_walltime_.count();
}

/**
 * @brief  Get the child time corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_child_walltime(size_t const hash) const
{
  auto& record = hash2record_const(hash);
  return record.child_walltime_.count();
}

/**
 * @brief  Get the region name corresponding to the input hash.
 * @param [in]  The hash corresponding to the region.
 */

std::string HashTable::get_region_name(size_t const hash) const
{
  auto& record = hash2record_const(hash);
  return record.region_name_;
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
  auto& record = hash2record_const(hash);
  return record.call_count_;
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
    auto& record = hash2record_const(profiler_hash_);
    return record.call_count_;
}

/**
 * @brief   Gets a reference to a region record for a given hash.
 * @param [in]  hash   The region
 * @returns     Region record reference.
 *
 */

RegionRecord& HashTable::hash2record(size_t const hash)
{
  // Assertions
  assert (lookup_table_.size() > 0);
  assert (lookup_table_.count(hash) > 0);

  return hashvec_[lookup_table_.at(hash)];
}

/**
 * @brief   Gets a const reference to a region record for a given hash. Can be
 *          called from const methods.
 * @param [in]  hash   The region
 * @returns     Region record reference.
 *
 */

RegionRecord const& HashTable::hash2record_const(size_t const hash) const
{
  // Assertions
  assert (lookup_table_.size() > 0);
  assert (lookup_table_.count(hash) > 0);

  return hashvec_[lookup_table_.at(hash)];
}


