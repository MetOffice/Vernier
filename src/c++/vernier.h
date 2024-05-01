/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   vernier.h
 * @brief  Top-level Vernier class.
 *
 * Contains the top-level class, whose methods are called from client code. Also
 * declares a top-level, global, vernier object.
 *
 */

#ifndef VERNIER_H
#define VERNIER_H

#include <iterator>
#include <vector>
#include <string_view>
#include <array>

#include <mpi.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#include "mpi_context.h"
#include "hashtable.h"

#define PROF_MAX_TRACEBACK_SIZE 1000

namespace meto
{

// Forward declarations. The definitions of these functions will require access
// to private methods.
extern "C" {
  void c_vernier_start_part1();
  void c_vernier_start_part2(long int& hash_out, char const* name);
}

/**
 * @brief  Top-level Vernier class.
 *
 * Maintains separate hashtables for each thread, and keeps a breadcrumb trail
 * of profiled regions.
 */

class Vernier
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
        TracebackEntry() = default;
        TracebackEntry(size_t, record_index_t, time_point_t, time_point_t);

        // Data members
        size_t         record_hash_;
        record_index_t record_index_;
        time_point_t   region_start_time_;
        time_point_t   calliper_start_time_;
    };

    // Default initialisation flag.  No explicit constructor, and pointless
    // to set this in the init() method.
    bool initialized_ = false;

    // Data members
    int max_threads_;

    // MPI Context
    MPIContext mpi_context_;

    // Static, threadprivate data members
    static time_point_t logged_calliper_start_time_;
    static int call_depth_;
    #pragma omp threadprivate(call_depth_, logged_calliper_start_time_)

    // Hashtables and tracebacks
    std::vector<HashTable>                                           thread_hashtables_;
    std::vector<std::array<TracebackEntry,PROF_MAX_TRACEBACK_SIZE>>  thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                        hashtable_iterator_t_;
    typedef std::vector<std::array<TracebackEntry,PROF_MAX_TRACEBACK_SIZE>>
                                                                     ::size_type traceback_index_t;

    // Private methods
    void   start_part1();
    size_t start_part2(std::string_view const);

  public:

    // Default constructor needed for `inline` global Vernier object.
    Vernier() = default;

    // Member functions
    void   init(MPI_Comm const client_comm_handle = MPI_COMM_WORLD);
    void   finalize();
    size_t start(std::string_view const);
    void   stop (size_t const);
    void   write();

    // Getters
    double                 get_total_walltime (size_t const, int const);
    double                 get_overhead_walltime (size_t const, int const);
    double                 get_self_walltime(size_t const hash, int const input_tid);
    double                 get_child_walltime(size_t const hash, int const input_tid) const;
    std::string            get_decorated_region_name(size_t const hash, int const input_tid) const;
    unsigned long long int get_call_count(size_t const hash, int const input_tid) const;
    unsigned long long int get_prof_call_count(int const input_tid) const;

    // Grant these functions access to private methods.
    void friend c_vernier_start_part1();
    void friend c_vernier_start_part2(long int& hash_out, char const* name);
};

// Declare global profiler
inline Vernier vernier;

} // End meto namespace
#endif
