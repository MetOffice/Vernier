/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "profiler.h"

#include <iostream>
#include <cassert>
#include <chrono>

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

    std::vector<std::pair<size_t,time_point_t>> new_list;
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
  auto start_time = std::chrono::steady_clock::now();
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
  auto stop_time = std::chrono::steady_clock::now();

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
  auto start_time = thread_traceback_[tid].back().second;
  auto deltatime  = stop_time - start_time;
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
 * @brief  Write profile information to file.
 *
 * @note   The default file that the profiler will spit information into is
 *         called "profiler-output.txt". There also exists the option to set a
 *         custom name via the environment variable "ProfOut".
 *
 */

void Profiler::write()
{
  // Find current MPI rank
  int current_rank;
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &current_rank);

  // Pickup environment variable filename if it exists, if not use the default
  // name of "profiler-output.txt". In either case, include the MPI rank in the
  // name of the file.
  const char* env_variable = std::getenv("PROFILER_OUTFILE");
  if (env_variable != NULL)
  {
    const char* user_filename = (env_variable + ("-" + std::to_string(current_rank))).c_str();
    output_stream.open(user_filename);
  }
  else
  {
    delete env_variable;
    std::string default_filename = "profiler-output-" + std::to_string(current_rank);
    output_stream.open(default_filename);
  }

  // Write to file
  for (auto& it : thread_hashtables_)
  {
    it.write(output_stream);
  }

  output_stream.flush();
  output_stream.close();
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

double Profiler::get_thread0_walltime(size_t const hash) const
{
  auto tid = static_cast<hashtable_iterator_t_>(0);
  return thread_hashtables_[tid].get_total_walltime(hash);
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
