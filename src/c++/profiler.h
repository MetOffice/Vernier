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
 * @brief  Struct to store values set during the start calliper, and needed in
 *         the stop calliper.
 */

struct StartCalliperValues
{
  public:

    // Constructors
    StartCalliperValues(double, double);

    // Data members
    double region_start_time_;
    double calliper_start_time_;
};

/**
 * @brief  Top-level profiler class.
 *
 * Maintains separate hashtables for each thread, and keeps a breadcrumb trail
 * of profiled regions.
 */

class Profiler
{
  private: 

    // Data members
    size_t profiler_hash_;
    int max_threads_;
    std::vector<HashTable>                                           thread_hashtables_;
    std::vector<std::vector<std::pair<size_t,StartCalliperValues>>>   thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                                     hashtable_iterator_t_;
    typedef std::vector<std::vector<std::pair<size_t,StartCalliperValues>>>::size_type pair_iterator_t_;

  public:

    // Constructors
    Profiler();

    // Member functions
    size_t start(std::string_view);
    void   stop (size_t const);
    void   write();
    double get_total_walltime (size_t const, int const);
    double get_overhead_time(int const);
};

// Declare global profiler
inline Profiler prof;

#endif
