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

RegionRecord::RegionRecord(size_t const region_hash, std::string_view region_name)
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
  hashvec_.reserve(1000);

  // Set the name and hash of the profiler entry.
  std::string const profiler_name = "__profiler__";
  profiler_hash_ = hash_function_(profiler_name);

  // Insert special entry for the profiler overhead time.
  assert (lookup_table_.count(profiler_hash_) == 0);
  hashvec_.emplace_back(profiler_hash_, profiler_name);
  profiler_index_ = hashvec_.size()-1;
  lookup_table_.emplace(profiler_hash_, profiler_index_);
  assert (lookup_table_.count(profiler_hash_) > 0);
}

/**
 * @brief  Inserts a new entry into the hashtable.
 *
 */

void HashTable::query_insert(std::string_view region_name, 
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
 * @brief
 *
 */

time_duration_t* HashTable::add_child_time(
                         record_index_t const record_index,
                         time_duration_t const child_walltime)
{
  auto& record = hashvec_[record_index];
  record.child_walltime_ += child_walltime;
  return &record.overhead_walltime_;
}

/**
 * @brief
 *
 */

time_duration_t& HashTable::increment_profiler_calls()
{
  auto& record = hashvec_[profiler_index_];
  ++record.call_count_;
  return record.total_walltime_;
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
  std::sort(begin(hashvec_), end(hashvec_),
      [](auto a, auto b) { return a.self_walltime_ > b.self_walltime_;});
  
  // Need to re-store the indices in the lookup table, since they will have all
  // moved around as a result of the above sort.
  for (auto it = begin(hashvec_); it != end(hashvec_); ++it) {
    auto current_index = it - hashvec_.begin();
    lookup_table_[it->region_hash_] =  static_cast<record_index_t>(current_index);
  }

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
 * @param [in] hash   The hash of the region to compute.
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
 *         code regions; includes updating the special profiling overhead entry.
 */

 void HashTable::prepare_computed_times_all()
 {

   auto total_overhead_time = time_duration_t::zero();

   // Loop over entries in the hashtable. This would include the special
   // profiler entry, but the RegionRecord constructor will have ensured that all
   // corresponding values are zero thus far.
   for (auto& [hash, index] : lookup_table_) {
     prepare_computed_times(hash2record(hash));
   }

   /// // Check that the special profiler hash entries are all zero, even after the
   /// // above loop.
   /// assert(lookup_table_.at(profiler_hash_).self_walltime_      == time_duration_t::zero());
   /// assert(lookup_table_.at(profiler_hash_).child_walltime_     == time_duration_t::zero());
   /// assert(lookup_table_.at(profiler_hash_).total_walltime_     == time_duration_t::zero());
   /// assert(lookup_table_.at(profiler_hash_).total_raw_walltime_ == time_duration_t::zero());

   // Set values for the profiler entry specifically in the hashtable.
   // lookup_table_.at(profiler_hash_).self_walltime_      = total_overhead_time_;
   // lookup_table_.at(profiler_hash_).child_walltime_     = time_duration_t::zero();
   // lookup_table_.at(profiler_hash_).total_walltime_     = total_overhead_time_;
   // lookup_table_.at(profiler_hash_).total_raw_walltime_ = total_overhead_time_;

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
 * @ brief
 *
 */

RegionRecord& HashTable::hash2record(size_t const hash)
{
  // Assertions
  assert (lookup_table_.size() > 0);
  assert (lookup_table_.count(hash) > 0);

  return hashvec_[lookup_table_.at(hash)];
}

RegionRecord const& HashTable::hash2record_const(size_t const hash) const
{
  // Assertions
  assert (lookup_table_.size() > 0);
  assert (lookup_table_.count(hash) > 0);

  return hashvec_[lookup_table_.at(hash)];
}


