/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   vernier_mpi.h
 * @brief  Dummy MPI stubs for Vernier.
 *
 * Provides stubs for MPI calls used by Vernier. Allow Vernier to be used
 * with non-MPI client codes.
 *
 */

#ifndef VERNIER_MPI_H
#define VERNIER_MPI_H

#ifdef USE_MPI
  #include <mpi.h>
#else
  #define MPI_COMM_NULL  0
  #define MPI_COMM_WORLD 1
  #define MPI_SUCCESS    0
  using MPI_Comm = int;
  using MPI_Fint = int;

  int MPI_Init([[maybe_unused]] int* const, [[maybe_unused]] char*** const);
  int MPI_Finalize(void);
  int MPI_Initialized(int* value); 

  int MPI_Abort([[maybe_unused]] MPI_Comm const comm, int const errcode);

  int MPI_Comm_f2c(MPI_Fint const);

  int MPI_Comm_free([[maybe_unused]] MPI_Comm* comm);
  int MPI_Comm_dup ([[maybe_unused]] MPI_Comm const comm, [[maybe_unused]] MPI_Comm* newcomm);

  int MPI_Comm_size([[maybe_unused]] MPI_Comm const comm, int* size);
  int MPI_Comm_rank([[maybe_unused]] MPI_Comm const comm, int* rank);
#endif

#endif
