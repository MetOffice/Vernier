/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "profiler.h"

#include <iostream>
#include <cassert>

/**
 * @brief Constructor for StartCalliperValues struct.
 *
 */

StartCalliperValues::StartCalliperValues(double start_time, 
                                         double start_calliper_deltatime)
  : start_time_(start_time)
  , start_calliper_deltatime_(start_calliper_deltatime)
  {}

/**
 * @brief Constructor
 *
 */

Profiler::Profiler(){

  // Set the maximum number of threads.
  max_threads_ = 1;
#ifdef _OPENMP
  max_threads_ = omp_get_max_threads();
#endif

  // Create vector of hash tables: one hashtable for each thread. Ensure that
  // each new table has an entry for the profiler itself. The value of
  // profiler_hash_ is the same for all threads, and is updated serially, so it
  // is OK to carry forward the value returned for the last thread.
  for (int tid=0; tid<max_threads_; ++tid)
  {
    HashTable new_table(tid);
    profiler_hash_ = new_table.insert_special("__profiler__");
    thread_hashtables_.push_back(new_table);

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
  double calliper_entry_time = omp_get_wtime();

  // Determine the thread number
  auto tid = static_cast<hashtable_iterator_t_>(0);
#ifdef _OPENMP
  tid = static_cast<hashtable_iterator_t_>(omp_get_thread_num());
#endif

  assert (tid <= thread_hashtables_.size());
  assert (tid <= thread_traceback_.size());

  // Insert this region into the thread's hash table.
  size_t const hash = thread_hashtables_[tid].query_insert(region_name);

  // Add routine to the traceback.
  double exit_time = omp_get_wtime();
  double calliper_deltatime = exit_time - calliper_entry_time;
  StartCalliperValues new_times = StartCalliperValues(exit_time, calliper_deltatime);
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

  // Note the time on entry to the profiler call, which matches the time at
  // which the profiled code region has stopped. 
  double calliper_entry_time = omp_get_wtime();
  double stop_time = calliper_entry_time;

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
    std::cout << "EMERGENCY STOP: hashes don't match." << "\n";
    exit (100);
  }

  // Get start calliper values needed for subsequent computation.
  StartCalliperValues& start_calliper_times = 
    thread_traceback_[tid].back().second;

  // Compute time spent in profiled subroutine.
  double deltatime = stop_time - start_calliper_times.start_time_;

  // Do the hashtable update for the child region.
  thread_hashtables_[tid].update(hash, deltatime);

  // Remove from the end of the list.
  thread_traceback_[tid].pop_back();

  // Account for time spent in the profiler itself. 
  double calliper_exit_time = omp_get_wtime();
  double calliper_deltatime = calliper_exit_time - calliper_entry_time;

  // Add child time and profiling overheads to parent
  if (! thread_traceback_[tid].empty()) {
    size_t parent_hash = thread_traceback_[tid].back().first;
    thread_hashtables_[tid].add_child_time(parent_hash, deltatime, calliper_deltatime);
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

/**
 * @brief  Get the the spent in the profiler itself, on the specified thread.
 *
 * @param[in] thread_id  The thread ID for which to return the overhead.
 */

double Profiler::get_overhead_time(int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_total_walltime(profiler_hash_);
}

