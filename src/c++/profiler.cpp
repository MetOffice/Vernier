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

  // Create vector of hash tables: one hashtable for each thread.
  for (int tid=0; tid<max_threads_; ++tid)
  {
    HashTable new_table(tid);
    thread_hashtables_.push_back(new_table);

    std::vector<std::pair<size_t,double>> new_list;
    thread_traceback_.push_back(new_list);
  }

  // Assertions
  assert ( static_cast<int> (thread_hashtables_.size()) == max_threads_);
  assert ( static_cast<int> (thread_traceback_.size() ) == max_threads_);

}

/**
 * @brief  Start timing.
 *
 */

size_t Profiler::start(std::string_view region_name)
{

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

  return hash;
}

/**
 * @brief  Stop timing.
 *
 */

void Profiler::stop(size_t const hash)
{

  // First job: log the stop time.
  double stop_time  = omp_get_wtime();

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
  return thread_hashtables_.at(tid).get_total_walltime(hash);
}

/**
 * @brief  Get the self walltime for the specified hash.
 *
 */

double Profiler::get_self_walltime(size_t const hash, int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_.at(tid).get_self_walltime(hash);
}

/**
 * @brief  Get the child walltime for the specified hash.
 *
 */

double Profiler::get_child_walltime(size_t const hash, int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_.at(tid).get_child_walltime(hash);
}

/**
 * @brief  Get the region name corresponding to the input hash.
 *
 */

std::string Profiler::get_region_name(size_t const hash, int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_.at(tid).get_region_name(hash);
}

/**
 * @brief  Gets the std::unordered_map "table_" hashtable.
 *
 */

const std::unordered_map<size_t,HashEntry>& Profiler::get_hashtable(int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_.at(tid).get_hashtable();
}

/**
 * @brief  Gets the inner layer vector in thread_traceback_.
 *
 */

const std::vector<std::pair<size_t,double>>& Profiler::get_inner_traceback_vector(int const input_tid)
{
  auto tid = static_cast<pair_iterator_t_>(input_tid);
  return thread_traceback_.at(tid);
}

/**
 * @brief  Gets the vector of (hash,HashEntry) pairs in Profiler.write() known as hashvec, the desired
 *         behaviour of which is to sort the entries from high to low self walltime.
 *
 */

const std::vector<std::pair<size_t, HashEntry>>& Profiler::get_hashvec(int const input_tid)
{
  auto tid = static_cast<hashtable_iterator_t_>(input_tid);
  return thread_hashtables_.at(tid).get_hashvec();
}

/**
 * @brief  Return the value of max_threads_
 *
 */

const int& Profiler::get_max_threads()
{
  return max_threads_;
}
