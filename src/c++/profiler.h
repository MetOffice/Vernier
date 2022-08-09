/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   profiler.h
 * @brief  Top-level profiler class.
 *
 * Contains the top-level class, whose methods are called from client code. Also
 * declares a top-level, global, profiler object.
 * 
 */

#ifndef PROFILER_H
#define PROFILER_H

#include <string_view>
#include <iterator>
#include <vector>

#include "omp.h"

#include "hashtable.h"

/**
 * @defgroup CPPAPI C++
 * @brief C++ API for the profiler
 */

/**
 * @brief  Top-level profiler class.
 *
 * Maintains separate hashtables for each thread, and keeps a breadcrumb trail
 * of profiled regions.
 *
 */

class Profiler
{
  private: 

    // Data members
    int max_threads_;
    std::vector<HashTable>                               thread_hashtables_;
    std::vector<std::vector<std::pair<size_t,double>>>   thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                        hashtable_iterator_t_;
    typedef std::vector<std::vector<std::pair<size_t,double>>>::size_type pair_iterator_t_;

  public:

    // Constructors
    /**
     * @ingroup CPPAPI
     * @brief Constructor for profiler class
     */
    Profiler();

    // Member functions

/**
 * @ingroup API
 * @brief  Start timing.
 *
 * @param[in] region_name A unique name for the region being timed.
 *
 */
    size_t start(std::string_view region_name);

/**
 * @ingroup API
 * @brief  Stop timing.
 *
 * @param[in] hash  The hash corresponding to the region to be stopped.
 */
    void   stop (size_t hash);

/**
 * @ingroup API
 * @brief  Write profile information.
 *
 */
    void   write();


/**
 * @ingroup API
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
    double get_thread0_walltime(size_t hash);
};

// Declare global profiler
inline Profiler prof;

#endif
