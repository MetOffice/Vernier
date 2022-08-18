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

    std::vector<std::pair<size_t,double>> new_list;
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
  double start_time = omp_get_wtime();
  thread_traceback_[tid].push_back(std::make_pair(hash, start_time));

  // Account for the time spent in the profiler itself. 
  double calliper_exit_time = omp_get_wtime();
  double calliper_deltatime = calliper_exit_time - calliper_entry_time;
  thread_hashtables_[tid].update(profiler_hash_, calliper_deltatime);

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

  // Increment the time for this
  double start_time = thread_traceback_[tid].back().second;
  double deltatime = stop_time - start_time;
  thread_hashtables_[tid].update(hash, deltatime);

  // Remove from the end of the list.
  thread_traceback_[tid].pop_back();

  // Add child time to parent
  if (! thread_traceback_[tid].empty()) {
    size_t parent_hash = thread_traceback_[tid].back().first;
    thread_hashtables_[tid].add_child_time(parent_hash, deltatime);
  }
 
  // Account for time spent in the profiler itself.
  double calliper_exit_time = omp_get_wtime();
  double calliper_deltatime = calliper_exit_time - calliper_entry_time;
  thread_hashtables_[tid].update(profiler_hash_, calliper_deltatime);

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
 * @brief  Get the total (inclusive) time of everything below the specified hash.
 *
 * @param[in] hash  The hash corresponding to the region of interest. 
 *
 * @note   This function is normally expected to be used to return the total
 *         wallclock time for whole run. Since this value is required only from
 *         thread 0, the function does not take a thread ID argument and returns
 *         the value for thread 0 only. Taking the hash argument avoids the need
 *         to store the top-level hash inside the profiler itself.
 *
 */

double Profiler::get_thread0_walltime(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_total_walltime(hash);
}

