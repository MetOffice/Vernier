/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.

 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   profiler_c.cpp
 * @brief  C-language interfaces for the profiler. 
 *
 * Neither Fortran or C can interface with C++ object constructs. Hence
 * C-language interfaces are needed to call the profiler from C and Fortran.
 *
 * Since Fortran is pass by reference, arguments are received as references (&).
 *
 */

#include "profiler.h"

#include <iostream>
#include <cstring>

extern "C" {
  void   c_profiler_start(long int&, char const*);
  void   c_profiler_stop (long int const&);
  void   c_profiler_write();
  double c_get_total_walltime(long int const&, int const&);
}

/**
 * @brief  Start timing a named region and return a unique handle.
 */

void c_profiler_start(long int& hash_out, char const* name)
{
  size_t hash = prof.start( name );

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_out), "Hash/Out size mismatch.");
  std::memcpy(&hash_out, &hash, sizeof(hash));
}

/**
 * @brief  Stop timing the region with the specified handle.
 */

void c_profiler_stop(long int const& hash_in)
{
  size_t hash;

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_in), "Hash/In size mismatch.");
  std::memcpy(&hash, &hash_in, sizeof(hash));

  prof.stop( hash );
}

/**
 * @brief Write the profile itself.
 */

void c_profiler_write()
{
  prof.write();
}

/**
 * @brief  Get the total wallclock time for the specified region on the
 *         specified thread.
 * @param[in] hash_in     The hash of the region of interest. 
 * @param[in] thread_id   Return the time for this thread ID.
 */

double c_get_total_walltime(long int const& hash_in, int const& thread_id)
{
  size_t hash;

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_in), "Hash/In size mismatch.");
  std::memcpy(&hash, &hash_in, sizeof(hash));

  return prof.get_total_walltime(hash, thread_id);
}

