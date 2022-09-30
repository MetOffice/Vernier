/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "profiler.h"
#include "prof_gettime.h"

#include <iostream>
#include <cassert>
#include <chrono>

/**
 * @brief Constructor for StartCalliperValues struct.
 *
 */

StartCalliperValues::StartCalliperValues(time_point_t region_start_time, 
                                         time_point_t calliper_start_time)
  : region_start_time_(region_start_time)
  , calliper_start_time_(calliper_start_time)
  {}

/**
 * @brief Constructor
 *
 */

Profiler::Profiler()
{

  // Set the maximum number of threads.
  max_threads_ = 1;
#ifdef _OPENMP
  max_threads_ = omp_get_max_threads();
#endif

  // Create vector of hash tables: one hashtable for each thread.
  for (int tid=0; tid<max_threads_; ++tid)
  {

    // Create a new table
    HashTable new_table(tid);
    thread_hashtables_.push_back(new_table);

    // Create a new list
    std::vector<std::pair<size_t,StartCalliperValues>> new_list;
    thread_traceback_.push_back(new_list);

  }

  // Assertions
  assert ( static_cast<int> (thread_hashtables_.size()) == max_threads_);
  assert ( static_cast<int> (thread_traceback_.size() ) == max_threads_);

}

/**
 * @brief  Start timing a profiled code region.
 * @param [in]  region_name   The code region name.
 * @returns     Unique hash for the code region being started.
 * @todo    Revisit profiling overhead measurement.  
 */

size_t Profiler::start(std::string_view region_name)
{

  // Note the time on entry to the profiler call.
  time_point_t calliper_start_time = prof_gettime();

  // Determine the thread number
  auto tid = static_cast<hashtable_iterator_t_>(0);
#ifdef _OPENMP
  tid = static_cast<hashtable_iterator_t_>(omp_get_thread_num());
#endif

  assert (tid <= thread_hashtables_.size());
  assert (tid <= thread_traceback_.size());

  // Insert this region into the thread's hash table.
  size_t const hash = thread_hashtables_[tid].query_insert(region_name);

  // Store the calliper and region start times.
  auto region_start_time = prof_gettime();
  StartCalliperValues new_times = StartCalliperValues(region_start_time, calliper_start_time);
  thread_traceback_[tid].push_back(std::make_pair(hash, new_times));

  return hash;
}

/**
 * @brief  Stop timing a profiled code region.
 * @param [in]   Hash of the profiled code region being stopped.
 * @todo    Revisit profiling overhead measurement.  
 */

void Profiler::stop(size_t const hash)
{

  // Log the region stop time.
  auto region_stop_time = prof_gettime();

  // Determine the thread number
  auto tid = static_cast<hashtable_iterator_t_>(0);
#ifdef _OPENMP
  tid = static_cast<hashtable_iterator_t_>(omp_get_thread_num());
#endif

  // Checks - which hash is last on the traceback list?
  size_t last_hash_on_list = thread_traceback_[tid].back().first;

  // Check that the hash is the one we expect. If it isn't, there is an error in
  // the instrumentation.
  if (hash != last_hash_on_list){
    std::cerr << "EMERGENCY STOP: hashes don't match." << "\n";
    exit (100);
  }

  // Get start calliper values needed for subsequent computation.
  StartCalliperValues& start_calliper_times = 
    thread_traceback_[tid].back().second;

  // Compute the region time
  auto deltatime = region_stop_time - start_calliper_times.region_start_time_;

  // Do the hashtable update for the child region.
  thread_hashtables_[tid].update(hash, deltatime);

  // Precompute times as far as possible. We just need the calliper stop time
  // later.
  auto temp_sum = start_calliper_times.calliper_start_time_ + deltatime;

  // Remove from the end of the list.
  thread_traceback_[tid].pop_back();

  // Prepare to add timings to parent
  if (! thread_traceback_[tid].empty()) {
   size_t parent_hash = thread_traceback_[tid].back().first;
   thread_hashtables_[tid].add_child_time(parent_hash, deltatime);

   // Account for time spent in the profiler itself. 
   auto calliper_stop_time = prof_gettime();
   auto interim_time = calliper_stop_time - temp_sum;
   thread_hashtables_[tid].add_overhead_time(parent_hash, interim_time);
  }
}

/**
 * @brief  Write profile information.
 *
 */

void Profiler::write()
{
  // Write each one
  for (auto& it : thread_hashtables_)
  {
    it.prepare_computed_times();
    it.write();
  }
}

/**
 * @brief  Get the total (inclusive) time of everything below the routine
 *         corresponding to the specified hash, on the specified thread.
 *
 * @param[in] hash  The hash corresponding to the region of interest. 
 * @param[in] thread_id  The thread ID for which to return the walltime.
 *
 * @note  Taking the hash argument avoids the need to store the top-level hash
 *        inside the profiler itself.
 */

double Profiler::get_total_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_total_walltime(hash);
}

double Profiler::get_total_raw_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_total_raw_walltime(hash);
}

double Profiler::get_overhead_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_overhead_walltime(hash);
}

/**
 * @brief  Get the self walltime for the specified hash.
 *
 */

double Profiler::get_self_walltime(size_t const hash, int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_self_walltime(hash);
}

/**
 * @brief  Get the child walltime for the specified hash.
 *
 */

double Profiler::get_child_walltime(size_t const hash, int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_child_walltime(hash);
}

/**
 * @brief  Get the region name corresponding to the input hash.
 *
 */

std::string Profiler::get_region_name(size_t const hash, int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_region_name(hash);
}

/**
 * @brief  Get the number of times the input hash region has been called on the
 *         input thread ID.
 *
 * @param[in] hash  The hash corresponding to the region of interest.
 * @param[in] tid   The ID corresponding to the thread of interest.
 *
 * @returns  Returns an integer corresponding to the number of times the
 *           region of interest has been called on the specified thread.
 *
 */

unsigned long long int Profiler::get_call_count(size_t const hash, int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_call_count(hash);
}

