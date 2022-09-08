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
    std::vector<HashTable> thread_hashtables_;
    std::vector<std::vector<std::pair<size_t,time_point_t>>> thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type hashtable_iterator_t_;
    typedef std::vector<std::vector<std::pair<size_t,time_point_t>>>::size_type pair_iterator_t_;

  public:

    // Constructors
    Profiler();

    // Member functions
    size_t start(std::string_view);
    void   stop (size_t const);
    void   write();

    // HashEntry getters
    double       get_thread0_walltime(size_t const hash) const;
    double       get_self_walltime(size_t const hash, int const input_tid);
    double       get_child_walltime(size_t const hash, int const input_tid) const;
    std::string  get_region_name(size_t const hash, int const input_tid) const;
    unsigned long long int get_region_call_count(size_t const hash, int const input_tid) const;

    // Getters that return a constant, referenced instance of a private data member
    std::unordered_map<size_t,HashEntry> const&        get_hashtable(int const input_tid) const;
    std::vector<std::pair<size_t,time_point_t>> const& get_inner_traceback_vector(int const input_tid) const;
    std::vector<std::pair<size_t,HashEntry>> const&    get_hashvec(int const input_tid) const;
    int get_max_threads() const;

};

// Declare global profiler
inline Profiler prof;

#endif
