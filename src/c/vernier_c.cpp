/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   vernier_c.cpp
 * @brief  C-language interfaces for Vernier.
 *
 * Neither Fortran or C can interface with C++ object constructs. Hence
 * C-language interfaces are needed to call Vernier from C and Fortran.
 *
 * Since Fortran is pass by reference, arguments are received as references (&).
 *
 */

#include "vernier.h"
#include "vernier_mpi.h"

#include <iostream>
#include <cstring>

extern "C" {
  void   c_vernier_init_default();
  void   c_vernier_init_comm(MPI_Fint const& client_comm_handle);
  void   c_vernier_finalize();
  void   c_vernier_start_part1();
  void   c_vernier_start_part2(long int&, char const*);
  void   c_vernier_stop (long int const&);
  void   c_vernier_write();
  double c_vernier_get_total_walltime(long int const&, int const&);
}

/**
 * @brief  Initialise Vernier with default MPI communicator. 
 * @details Default communicator is specified in the underlying C++ code.
 */

void c_vernier_init_default()
{
  meto::vernier.init();
}

/**
 * @brief  Initialise Vernier, using the specified MPI communicator.
 * @param [in] The Fortran communicator handle.
 */

void c_vernier_init_comm(MPI_Fint const& client_comm_handle)
{
  MPI_Comm local_handle = MPI_Comm_f2c(client_comm_handle);
  meto::vernier.init(local_handle);
}

/**
 * @brief  Finalise Vernier.
 */

void c_vernier_finalize() 
{
  meto::vernier.finalize();
}

/**
 * @brief  Start timing, part 1 of 2.
 */

void meto::c_vernier_start_part1()
{
  meto::vernier.start_part1();
}

/**
 * @brief  Start timing, part 2 of 2. a named region and return a unique handle.
 * @param [in]   name      The region name, null terminated.
 * @param [out]  hash_out  The returned unique hash for this region.
 */

void meto::c_vernier_start_part2(long int& hash_out, char const* name)
{
  size_t hash = meto::vernier.start_part2( name );

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_out), "Hash/Out size mismatch.");
  std::memcpy(&hash_out, &hash, sizeof(hash));
}

/**
 * @brief  Stop timing the region with the specified handle.
 */

void c_vernier_stop(long int const& hash_in)
{
  size_t hash;

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_in), "Hash/In size mismatch.");
  std::memcpy(&hash, &hash_in, sizeof(hash));

  meto::vernier.stop( hash );
}

/**
 * @brief Write the profile itself.
 */

void c_vernier_write()
{
  meto::vernier.write();
}

/**
 * @brief  Get the total wallclock time for the specified region on the
 *         specified thread.
 * @param[in] hash_in     The hash of the region of interest. 
 * @param[in] thread_id   Return the time for this thread ID.
 */

double c_vernier_get_total_walltime(long int const& hash_in, int const& thread_id)
{
  size_t hash;

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_in), "Hash/In size mismatch.");
  std::memcpy(&hash, &hash_in, sizeof(hash));

  return meto::vernier.get_total_walltime(hash, thread_id);
}

