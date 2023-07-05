/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashtable.h"
#include "hashvec_handler.h"

#include <cassert>
#include <iterator>

#define PROF_HASHVEC_RESERVE_SIZE 1000

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

  // Set the name and hash of the profiler entry.
  std::string const profiler_name = "__profiler__@" + std::to_string(tid);

  // Insert special entry for the profiler overhead time.
  query_insert(profiler_name, profiler_hash_, profiler_index_);
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

  // Does the entry exist already?
  if (auto search = lookup_table_.find(hash); search != lookup_table_.end())
  {
    record_index = search->second;
  }
  
  // If not, create new entry.
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
 * @param [in] record_index  The index in hashvec_ corresponding to the
 *                           profiled region.
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
 * @brief  Add in time spent calling child regions. Also retuns a pointer
 *         to the overhead time so that it can be incremented downstream,
 *         outside this function, with minimal additional overhead. 
 * @param [in]  record_index   The index corresponding to the region record.
 * @param [in]  time_delta     The time spent in the child region.
 * @param [out] overhead_time_ptr  Pointer to the profiling overhead time 
 *                                 incurred by calling children of this region. 
 */

void HashTable::add_child_time_to_parent(
                    record_index_t  const parent_index,
                    time_duration_t const child_walltime,
                    time_duration_t*& overhead_time_ptr)
{
  auto& record = hashvec_[parent_index];
  record.child_walltime_ += child_walltime;
  overhead_time_ptr = &record.overhead_walltime_;
}

/**
 * @brief Increment the number of calls to the profiler callipers. Also returns
 *        a pointer to the total profiling overhead time so that it can be
 *        incremented downstream, outside this function, with minimal
 *        additional overhead.
 * @param [out] overhead_time_ptr  Pointer to the total profiling overhead time 
 *                                 incurred by calling every set of profiler
 *                                 calls. 
 */

void HashTable::add_profiler_call(time_duration_t*& overhead_time_ptr) {
  auto& record = hashvec_[profiler_index_];
  ++record.call_count_;
  overhead_time_ptr = &record.total_walltime_;
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
  sync_lookup();

}

/**
 * @brief   Evaluates times derived from other times measured, for a particular
 *          code region.
 * @details Times computed are: the region self time and the total time minus
 *          directly incurred profiling overhead costs.
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
 * @brief  Appends table_ onto the end of an input hashvec. 
 * @param[inout] hashvec_handler  HashVecHandler object containing the
 *                                hashvec to amend.
 * 
 */

void HashTable::append_to(HashVecHandler& hashvec_handler)
{
  // Compute overhead and self times before appending
  prepare_computed_times_all();

  // Loop over entries in the hashtable.
  for (auto& [hash, index] : lookup_table_) {
    prepare_computed_times(hash2record(hash));
  }

  // Erase profiler entry if call count is zero.
  if(hash2record(profiler_hash_).call_count_ == 0) {
    erase_record(profiler_hash_);
  }

  // Sync-up the lookup table and hashvec.
  sync_lookup();
  
  // Append hashvec to that passed through the argument list.
  hashvec_handler.append(hashvec_);
}

/**
 * @brief Erases record from the hashvec and lookup table.
 * @param[in] hash   Hash of the record to erase.
 *
 */

void HashTable::erase_record(size_t const hash)
{

  // Find the lookup table (hashtable) iterator.
  auto iterator    = lookup_table_.find(hash);
  auto const index = lookup_table_.at(hash);

  // Get the hashvec iterator from the index
  auto record_iterator = begin(hashvec_);
  std::advance( record_iterator, index);

  // Erase from both the hashvec and the lookup table.
  hashvec_.erase(record_iterator);
  lookup_table_.erase(iterator);

}

/**
 * @brief Updates vector indices stored in the lookup table.
 *
 */

void HashTable::sync_lookup()
{
  // Need to re-store the indices in the lookup table, since there will be a gap
  // as a result of the erase().
  for (auto it = begin(hashvec_); it != end(hashvec_); ++it) {
    auto current_index = it - hashvec_.begin();
    lookup_table_[it->region_hash_] =  static_cast<record_index_t>(current_index);
  }
}

/**
 * @brief  Get the total (inclusive) time corresponding to the input hash.
 * @param[in]  hash  Fetches the total wallclock time for the region
 *                   with this hash.
 *
 */

double HashTable::get_total_walltime(size_t const hash) const
{
  auto& record = hash2record(hash);

  return record.total_walltime_.count();
}

/**
 * @brief  Get the total time of the specified region, minus profiling overheads
 *         incurred by calling direct children.
 * @param [in] hash  The hash corresponding to the region.
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
 * @param [in] hash  The hash corresponding to the region.
 */

double HashTable::get_overhead_walltime(size_t const hash) const
{
  auto& record = hash2record(hash);
  return record.overhead_walltime_.count();
}

/**
 * @brief  Get the profiler self (exclusive) time corresponding to the input hash.
 * @param [in] hash  The hash corresponding to the region.
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
 * @param [in] hash  The hash corresponding to the region.
 * @note   This time is derived from other measured times, therefore a to
 *         `prepare_computed_times` is need to update its value. 
 */

double HashTable::get_child_walltime(size_t const hash) const
{
  auto& record = hash2record(hash);
  return record.child_walltime_.count();
}

/**
 * @brief  Get the region name corresponding to the input hash.
 * @param [in] hash  The hash corresponding to the region.
 */

std::string HashTable::get_region_name(size_t const hash) const
{
  auto& record = hash2record(hash);
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
  auto& record = hash2record(hash);
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
    auto& record = hash2record(profiler_hash_);
    assert (lookup_table_.count(profiler_hash_) > 0);
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
  return hashvec_[lookup_table_.at(hash)];
}

/**
 * @brief   Gets a const reference to a region record for a given hash. Can be
 *          called from const methods.
 * @param [in]  hash   The region
 * @returns     Region record reference.
 *
 */

RegionRecord const& HashTable::hash2record(size_t const hash) const
{
  return hashvec_[lookup_table_.at(hash)];
}


