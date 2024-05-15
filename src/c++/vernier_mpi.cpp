/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <cstdlib>

#include "vernier_mpi.h"

#ifndef USE_MPI

/**
 * @brief Stub for MPI_Init
 * @returns MPI success code.
 */

int MPI_Init([[maybe_unused]] int*, [[maybe_unused]] char***)
{
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Finalize
 * @returns MPI success code.
 */

int MPI_Finalize(void)
{
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Abort
 * @returns MPI success code.
 */

int MPI_Abort([[maybe_unused]] MPI_Comm const comm, int const errcode)
{
  std::exit (errcode);
}

/**
 * @brief Stub for MPI_Comm_f2c
 * @returns MPI communicator handle.
 */

int MPI_Comm_f2c(MPI_Fint comm)
{
  return comm;
}

/**
 * @brief Stub for MPI_Initialized
 * @returns MPI success code.
 */

int MPI_Initialized(int* value)
{
  *value=MPI_COMM_WORLD;
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Comm_free
 * @returns MPI success code.
 */

int MPI_Comm_free([[maybe_unused]] MPI_Comm* comm)
{
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Comm_dup
 * @returns MPI success code.
 */

int MPI_Comm_dup (MPI_Comm comm, MPI_Comm* newcomm)
{
  *newcomm = comm; 
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Comm_size
 * @returns MPI success code.
 */

int MPI_Comm_size([[maybe_unused]] MPI_Comm comm, int* size)
{
  *size = 1;
  return MPI_SUCCESS;
}

/**
 * @brief Stub for MPI_Comm_rank
 * @returns MPI success code.
 */

int MPI_Comm_rank([[maybe_unused]] MPI_Comm comm, int* rank)
{
  *rank = 0;
  return MPI_SUCCESS;
}

#endif
