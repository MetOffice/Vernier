/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2023 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "mpi_context.h"
#include <cassert>

/**
 * @brief  Constructor for an MPI context.
 * @param [in] client_comm_handle  The MPI communicator handle passed down from
 *                                 the client code.
 * @details This constructor does not initialise MPI.
 * @returns A new MPIContext object.
 * @note   The client communicator handle defaults to MPI_COMM_NULL. If no actual
 *         argument is supplied, and MPI is initialised, then Vernier
 *         will use MPI_COMM_WORLD by default.
 */

meto::MPIContext::MPIContext(MPI_Comm client_comm_handle)
{

  // Check that the storage is correctly null first.
  assert(comm_handle_ == MPI_COMM_NULL);
  assert(comm_rank_   == -1);
  assert(comm_size_   == -1);

  int mpi_is_initialised;
  MPI_Initialized(&mpi_is_initialised);

  if (mpi_is_initialised) {
    if (client_comm_handle != MPI_COMM_NULL) {
      MPI_Comm_dup(client_comm_handle, &comm_handle_);
    }
    else {
      MPI_Comm_dup(MPI_COMM_WORLD, &comm_handle_);
    }
    MPI_Comm_rank(comm_handle_, &comm_rank_);
    MPI_Comm_size(comm_handle_, &comm_size_);
  }
  else {
    comm_handle_ = MPI_COMM_NULL;
    comm_rank_   = 0;
    comm_size_   = 1;
  }

}

/**
 * @brief  Destructor for an MPI context.
 * @note   Since the constructor duplicated an MPI communicator, creating a new
 *         communicator handle in the process, this destructor needs to free that
 *         communicator handle.
 */

// meto::MPIContext::~MPIContext()
// {
// 
//   if (comm_handle_ != MPI_COMM_WORLD &&
//       comm_handle_ != MPI_COMM_NULL){
//         MPI_Comm_free(&comm_handle_);
//   }
// 
//   comm_handle_ = MPI_COMM_NULL;
//   comm_rank_   = -1;
//   comm_size_   = -1;
// 
// }

/**
 * @brief Gets the MPI rank from an MPIContext object.
 * @returns The MPI rank.
 */

int meto::MPIContext::get_rank()
{
    return comm_rank_;
}

/**
 * @brief Gets the size of the MPI communicator from an MPIContext object.
 * @returns The MPI communicator size.
 */

int meto::MPIContext::get_size()
{
    return comm_size_;
}

