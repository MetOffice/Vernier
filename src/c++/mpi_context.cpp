/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2023 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "mpi_context.h"

// Constructor
meto::MPIContext::MPIContext()
{

  int mpi_is_initialised;
  MPI_Initialized(&mpi_is_initialised);

  if (mpi_is_initialised) {
    MPI_Comm_dup(MPI_COMM_WORLD, &comm_handle_);
    MPI_Comm_rank(comm_handle_, &comm_rank_);
    MPI_Comm_size(comm_handle_, &comm_size_);
  }
  else {
    comm_handle_ = MPI_COMM_NULL;
    comm_rank_   = 0;
    comm_size_   = 1;
  }
}

// Get rank
int meto::MPIContext::get_rank()
{
    return comm_rank_;
}

// Get size
int meto::MPIContext::get_size()
{
    return comm_size_;
}
