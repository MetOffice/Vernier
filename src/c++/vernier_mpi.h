/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#ifndef VERNIER_MPI_H
#define VERNIER_MPI_H

#ifdef USE_MPI
  #include <mpi.h>
#else
  #define MPI_COMM_NULL  0
  #define MPI_COMM_WORLD 1
  using MPI_Comm = int;
  using MPI_Fint = int;

  int MPI_Init([[maybe_unused]] int*, [[maybe_unused]] char***);
  int MPI_Finalize(void);
  int MPI_Initialized(int* value); 

  int MPI_Abort([[maybe_unused]] MPI_Comm const comm, int const errcode);

  int MPI_Comm_f2c(MPI_Fint);

  int MPI_Comm_free([[maybe_unused]] MPI_Comm* comm);
  int MPI_Comm_dup ([[maybe_unused]] MPI_Comm comm, [[maybe_unused]] MPI_Comm* newcomm);

  int MPI_Comm_size([[maybe_unused]] MPI_Comm comm, int* size);
  int MPI_Comm_rank([[maybe_unused]] MPI_Comm comm, int* rank);
#endif

#endif
