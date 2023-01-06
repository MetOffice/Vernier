/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

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
#include <array>
#include <omp.h>

#include "hashtable.h"

#define PROF_MAX_TRACEBACK_SIZE 1000

/**
 * @brief  Top-level profiler class.
 *
 * Maintains separate hashtables for each thread, and keeps a breadcrumb trail
 * of profiled regions.
 */

class Profiler
{
  private:

    /**
     * @brief  Struct to store values set during the start calliper, and needed in
     *         the stop calliper.
     */

    struct TracebackEntry
    {
      public:

        // Constructors
        TracebackEntry();
        TracebackEntry(size_t, record_index_t, time_point_t, time_point_t);

        // Data members
        size_t         record_hash_;
        record_index_t record_index_;
        time_point_t   region_start_time_;
        time_point_t   calliper_start_time_;
    };

    // Data members
    int max_threads_;

    std::vector<HashTable>                                           thread_hashtables_;
    std::vector<std::array<TracebackEntry,PROF_MAX_TRACEBACK_SIZE>>  thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                        hashtable_iterator_t_;
    typedef std::vector<std::array<TracebackEntry,PROF_MAX_TRACEBACK_SIZE>>
                                                                     ::size_type traceback_index_t;

  public:

    // Constructors
    Profiler();

    // Member functions
    size_t start(std::string_view const);
    void   start_part1();
    size_t start_part2(std::string_view const);
    void   stop (size_t const);
    void   write();

    // Getters
    double                 get_total_walltime (size_t const, int const);
    double                 get_total_raw_walltime (size_t const, int const);
    double                 get_overhead_walltime (size_t const, int const);
    double                 get_self_walltime(size_t const hash, int const input_tid);
    double                 get_child_walltime(size_t const hash, int const input_tid) const;
    std::string            get_region_name(size_t const hash, int const input_tid) const;
    unsigned long long int get_call_count(size_t const hash, int const input_tid) const;
    unsigned long long int get_prof_call_count(int const input_tid) const;

};

// Declare global profiler
inline Profiler prof;

#endif
