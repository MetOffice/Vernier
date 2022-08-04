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
    std::vector<HashTable>                             thread_hashtables_;
    std::vector<std::vector<std::pair<size_t,double>>> thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                        hashtable_iterator_t_;
    typedef std::vector<std::vector<std::pair<size_t,double>>>::size_type pair_iterator_t_;

  public:

    // Constructors
    Profiler();

    // Member functions
    size_t start(std::string_view);
    void   stop (size_t const);
    void   write();
    bool   is_table_empty();
    
    // Getters
    double      get_thread0_walltime(size_t const);
    double      get_thread0_self_walltime(size_t const);
    double      get_thread0_child_walltime(size_t const); 
    std::string get_thread0_region_name(size_t const);

    size_t get_thread_hashtables_size(); 
    size_t get_thread_traceback_size();
    int get_max_threads();
    size_t get_hashtable_count(size_t const);
    size_t hashtable_query_insert(std::string_view);
      

    std::vector<std::pair<size_t, HashEntry>> get_hashvec();
    std::vector<std::pair<size_t,double>> get_traceback_vector();
};

// Declare global profiler
inline Profiler prof;

#endif
