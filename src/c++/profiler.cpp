/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashvec_handler.h"
#include "profiler.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <omp.h>

// Initialize static data members.
int Profiler::call_depth_ = -1;
time_point_t Profiler::logged_calliper_start_time_{};

/**
 * @brief Constructor for TracebackEntry struct.
 * @param [in]  record_hash   The hash of the region name.
 * @param [in]  record_index  The index of the region record.
 * @param [in]  region_start_time  The clock measurement just before leaving the
 *                                 start calliper.
 * @param [in]  calliper_start_time The clock measurement on entry to the start
 *                                  calliper.
 *
 */

Profiler::TracebackEntry::TracebackEntry(
                                   size_t         record_hash,
                                   record_index_t record_index,
                                   time_point_t region_start_time, 
                                   time_point_t calliper_start_time)
  : record_hash_        (record_hash)
  , record_index_       (record_index)
  , region_start_time_  (region_start_time)
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
    std::array<TracebackEntry, PROF_MAX_TRACEBACK_SIZE> new_list;
    thread_traceback_.push_back(new_list);

  }

  // Assertions
  assert ( static_cast<int> (thread_hashtables_.size()) == max_threads_);
  assert ( static_cast<int> (thread_traceback_.size() ) == max_threads_);

}

/**
 * @brief   Start timing a profiled code region.
 * @details Calls both part1 and part2 start routines in succession.
 * @param [in]  region_name   The code region name.
 * @returns     Unique hash for the code region being started.
 */

size_t Profiler::start(std::string_view const region_name)
{
  start_part1();
  auto hash = start_part2(region_name);
  return hash;
}

/**
 * @brief  Start timing a profiled code region, part 1 of 2: make a 
 *         threadprivate note of the time.
 */

void Profiler::start_part1()
{
  // Store the calliper start time, which is used in part2.
  logged_calliper_start_time_ = prof_gettime();
}

/**
 * @brief  Start timing a profiled code region, part 2 of 2.
 * @param [in]  region_name   The code region name.
 * @returns     Unique hash for the code region being started.
 */

size_t Profiler::start_part2(std::string_view const region_name)
{
  // Determine the thread number
  auto tid = static_cast<hashtable_iterator_t_>(0);
#ifdef _OPENMP
  tid = static_cast<hashtable_iterator_t_>(omp_get_thread_num());
#endif
  int tid_int = static_cast<int>(tid);

  assert (tid <= thread_hashtables_.size());
  assert (tid <= thread_traceback_.size());

  size_t hash;
  record_index_t record_index;
  thread_hashtables_[tid].query_insert(region_name, tid_int, hash, record_index);

  // Store the calliper and region start times.
  ++call_depth_;
  if (call_depth_ < PROF_MAX_TRACEBACK_SIZE){
    auto call_depth_index = static_cast<traceback_index_t>(call_depth_);
    auto region_start_time = prof_gettime();
    thread_traceback_[tid].at(call_depth_index) 
       = TracebackEntry(hash, record_index, region_start_time, logged_calliper_start_time_);
  }
  else {
    std::cerr << "EMERGENCY STOP: Traceback array exhausted." << "\n";
    exit (102);
  }
  return hash;
}

/**
 * @brief  Stop timing a profiled code region.
 * @param [in] hash   Hash of the profiled code region being stopped.
 * @note  The calliper time (spent in the profiler) is measured by
 *        differencing the beginning of the start calliper from the end of the stop
 *        calliper, and subtracting the measured region time. Hence larger
 *        absolute times are being measured, which are less likely to suffer
 *        fractional error from precision limitations of the clock.   
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

  // Check that we have called a start calliper before the stop calliper.
  // If not, then the call depth would be -1.
  if (call_depth_ < 0) {
    std::cerr << "EMERGENCY STOP: stop called before start calliper." << "\n";
    exit (101);
  }

 // Get reference to the traceback entry.
 auto call_depth_index = static_cast<traceback_index_t>(call_depth_);
 auto& traceback_entry = thread_traceback_[tid].at(call_depth_index);

  // Check: which hash is last on the traceback list?
  size_t last_hash_on_list = traceback_entry.record_hash_;
  if (hash != last_hash_on_list){
    std::cerr << "EMERGENCY STOP: hashes don't match." << "\n";
    exit (100);
  }

  // Compute the region time
  auto region_duration = region_stop_time - traceback_entry.region_start_time_;

  // Do the hashtable update for the child region.
  thread_hashtables_[tid].update(traceback_entry.record_index_, region_duration);

  // Precompute times as far as possible. We just need the calliper stop time
  // later.
  //   (t4-t1) = calliper time + region duration
  //   (t3-t2) = region_duration
  //   calliper_time = (t4-t1) - (t3-t2)  = t4 - ( t3-t2 + t1)
  auto temp_sum = traceback_entry.calliper_start_time_ + region_duration;

  // The sequence of code that follows is aimed at leaving only minimal and
  // simple operations after the call to prof_gettime().
  time_duration_t* parent_overhead_time_ptr   = nullptr;
  time_duration_t* profiler_overhead_time_ptr = nullptr;

  // Acquire parent pointers
  if (call_depth_ > 0){
    auto parent_depth = static_cast<traceback_index_t>(call_depth_-1);
    record_index_t parent_index = thread_traceback_[tid].at(parent_depth).record_index_;
    thread_hashtables_[tid].add_child_time_to_parent(
                              parent_index, region_duration,
                              parent_overhead_time_ptr);
  }

  // Increment profiler calls, and get a pointer to the total overhead time.
  thread_hashtables_[tid].add_profiler_call(profiler_overhead_time_ptr);

  // Decrement index to last entry in the traceback.
  --call_depth_;

  // Account for time spent in the profiler itself. 
  auto calliper_stop_time = prof_gettime();
  auto calliper_time = calliper_stop_time - temp_sum;

  // Increment the overhead time specific to this region, incurred when calling
  // direct children, and also the overall profiling overhead time.
  // Being outside the stop calliper, these operations need to be as cheap
  // as possible.
  if(parent_overhead_time_ptr) { *parent_overhead_time_ptr += calliper_time; }
  *profiler_overhead_time_ptr += calliper_time;
}

/**
 * @brief  Write profile information to file.
 *
 * @note  The default output file seedname is  "profiler-output". There also
 *        exists the option to set a custom name via an environment variable.
 *
 */

void Profiler::write()
{
  // Create hashvec handler object and feed in data from thread_hashtables_
  HashVecHandler output_data;
  for (auto& table : thread_hashtables_)
  {
    table.append_to(output_data);
  }

  // Sort hashvec from high to low self walltimes then write
  output_data.sort();
  output_data.write();
}


/**
 * @brief  Get the total (inclusive) time taken by a region and everything below it.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] thread_id  The thread ID for which to return the walltime.
 *
 */

double Profiler::get_total_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_total_walltime(hash);
}

/**
 * @brief  Get the total (inclusive) time taken by a region, and everything below it,
 *         minus the profiling overheads for calls to direct child regions.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] thread_id  The thread ID for which to return the walltime.
 *
 */

double Profiler::get_total_raw_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_total_raw_walltime(hash);
}

/**
 * @brief  Get the profiling overhead time experienced by a region, 
 *         as incurred by calling child regions.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] thread_id  The thread ID for which to return the walltime.
 *
 */

double Profiler::get_overhead_walltime(size_t const hash, int const thread_id)
{
  auto tid = static_cast<hashtable_iterator_t_>(thread_id);
  return thread_hashtables_[tid].get_overhead_walltime(hash);
}

/**
 * @brief  Get the self (exclusive) time spent executing a region, minus the
 *         cost of child regions.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] input_tid  The thread ID for which to return the walltime.
 *
 */

double Profiler::get_self_walltime(size_t const hash, int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_self_walltime(hash);
}

/**
 * @brief  Get the time spent executing children of a region, including 
 *         the time taken by their descendents.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] input_tid  The thread ID for which to return the walltime.
 *
 * @note  This time does not include profiling overhead costs incurred directly
 *        by the region.
 *
 */

double Profiler::get_child_walltime(size_t const hash, int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_child_walltime(hash);
}

/**
 * @brief  Get the name of a region corresponding to a given hash.
 *
 * @param[in] hash       The hash corresponding to the region of interest. 
 * @param[in] input_tid  The thread ID for which to return the walltime.
 *
 * @note  The thread ID is included to future-proof against the possibility of
 *        including the thread ID in hashed strings. Hence the hash may not be
 *        the same on each thread.
 *
 */

std::string Profiler::get_decorated_region_name(size_t const hash,
                                                int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_decorated_region_name(hash);
}

/**
 * @brief  Get the number of times the input hash region has been called on the
 *         input thread ID.
 *
 * @param[in] hash       The hash corresponding to the region of interest.
 * @param[in] input_tid  The ID corresponding to the thread of interest.
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

/**
 * @brief  Get the number of calliper pairs called on the specified thread.
 *
 * @param[in] input_tid   The ID corresponding to the thread of interest.
 *
 * @returns  Returns an integer corresponding to the number of times the
 *           region of interest has been called on the specified thread.
 *
 */

unsigned long long int Profiler::get_prof_call_count(int const input_tid) const
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_[tid].get_prof_call_count();
}

