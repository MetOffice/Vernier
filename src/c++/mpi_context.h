/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   mpi_context.h
 *  @brief  Defines an MPIContext class.
 *
 *  In order to reduce the proliferation of MPI throughout the codebase, and to
 *  ensure that required information - such as MPI rank - is available at
 *  appropriate levels in the code, we define an MPIContext class which we know
 *  contains any data/methods we might need.
 *
 */

#ifndef VERNIER_MPI_CONTEXT_H
#define VERNIER_MPI_CONTEXT_H

#include <string_view>
#include <unordered_map>

#include "hashvec.h"
#include "vernier_gettime.h"
#include "vernier_mpi.h"

#define MPI_CONTEXT_NULL_STRING "mpi_context_null_string"

namespace meto {

/**
 * @brief  Provides a wrapper for MPI functionality.
 */

class MPIContext {

private:
  MPI_Comm comm_handle_;
  int comm_size_;
  int comm_rank_;
  bool initialized_;
  std::string tag_;

public:
  // Constructor
  MPIContext();

  // Init and finalize
  bool is_initialized();
  void init(MPI_Comm, std::string_view tag);
  void finalize();
  void reset();

  // Getters
  int get_size();
  int get_rank();
  std::string get_tag() const;
};

} // namespace meto

#endif
