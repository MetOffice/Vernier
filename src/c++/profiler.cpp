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

  // Increment the total walltime for this
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
 *         Note: for thread 0 only.  This function provides an accessor to the
 *         total wallclock time as measured by the profiler.
 *
 */

double Profiler::get_thread0_walltime(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_total_walltime(hash);
}

double Profiler::get_thread0_self_walltime(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_self_walltime(hash);
}

double Profiler::get_thread0_child_walltime(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_child_walltime(hash);
}

std::string Profiler::get_thread0_region_name(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0); 
  return thread_hashtables_[tid].get_region_name(hash);
}

size_t Profiler::get_thread_hashtables_size()
{
  return thread_hashtables_.size();
}

size_t Profiler::get_thread_traceback_size()
{
  return thread_traceback_.size();
}

int Profiler::get_max_threads()
{
  return max_threads_;
}

size_t Profiler::get_hashtable_count(size_t const hash)
{
  auto tid = static_cast<hashtable_iterator_t_>(0); 
  return thread_hashtables_[tid].get_hashtable_count(hash);
}

size_t Profiler::hashtable_query_insert(std::string_view region_name) 
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].query_insert(region_name);
}

bool Profiler::is_table_empty()
{
  auto tid = static_cast<hashtable_iterator_t_>(0); 
  return thread_hashtables_[tid].is_table_empty();
}

std::vector<std::pair<size_t, HashEntry>> Profiler::get_hashvec()
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_hashvec();
}

std::pair<size_t,double> Profiler::get_final_traceback_pair()
{
  auto tid = static_cast<pair_iterator_t_>(0);
  return thread_traceback_[tid].back();
}

