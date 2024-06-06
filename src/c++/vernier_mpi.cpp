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

int MPI_Init([[maybe_unused]] int *const, [[maybe_unused]] char ***const) {
  return MPI_SUCCESS;
}

/**
 * @brief  Stub for MPI_Finalize
 * @returns MPI success code.
 */

int MPI_Finalize(void) { return MPI_SUCCESS; }

/**
 * @brief  Stub for MPI_Abort
 * @param [in] comm     MPI communicator handle.
 * @param [in] errcode  Error code with which to exit.
 * @returns MPI success code.
 */

int MPI_Abort([[maybe_unused]] MPI_Comm const comm, int const errcode) {
  std::exit(errcode);
}

/**
 * @brief  Stub for MPI_Comm_f2c
 *
 * @param [in] MPI communicator handle. (Fortran)
 * @returns MPI communicator handle.
 */

int MPI_Comm_f2c(MPI_Fint const comm) { return comm; }

/**
 * @brief  Stub for MPI_Initialized
 * @param [out] flag  Initialised flag set to 1.
 * @returns MPI success code.
 */

int MPI_Initialized(int *flag) {
  *flag = 1;
  return MPI_SUCCESS;
}

/**
 * @brief  Stub for MPI_Comm_free
 * @param [out] comm  MPI communicator handle.
 * @returns MPI success code.
 */

int MPI_Comm_free([[maybe_unused]] MPI_Comm *comm) {
  *comm = MPI_COMM_NULL;
  return MPI_SUCCESS;
}

/**
 * @brief  Stub for MPI_Comm_dup
 * @param [in]  comm     MPI communicator handle.
 * @param [out] newcomm  MPI communicator handle.
 * @returns MPI success code.
 * @note  Adds 1 to the input communicator handle so that
 *        input and output handles are not the same.
 */

int MPI_Comm_dup(MPI_Comm const comm, MPI_Comm *newcomm) {
  *newcomm = comm + 1;
  return MPI_SUCCESS;
}

/**
 * @brief  Stub for MPI_Comm_size
 * @param [in]  comm  MPI communicator handle.
 * @param [out] size  Size of communicator, set to 1.
 * @returns MPI success code.
 */

int MPI_Comm_size([[maybe_unused]] MPI_Comm const comm, int *size) {
  *size = 1;
  return MPI_SUCCESS;
}

/**
 * @brief  Stub for MPI_Comm_rank
 * @param [in]  comm  MPI communicator handle.
 * @param [out] rank  Rank within communicator, set to 0.
 * @returns MPI success code.
 */

int MPI_Comm_rank([[maybe_unused]] MPI_Comm const comm, int *rank) {
  *rank = 0;
  return MPI_SUCCESS;
}

#endif
