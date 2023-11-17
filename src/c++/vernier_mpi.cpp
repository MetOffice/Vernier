/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "vernier_mpi.h"

#ifndef USE_MPI

int MPI_Init([[maybe_unused]] int*, [[maybe_unused]] char***){return 0;}
int MPI_Finalize(void){return 0;}

int MPI_Comm_f2c(MPI_Fint comm) { return comm; }
int MPI_Initialized(int* value) { *value=1; return 0; }
int MPI_Comm_free([[maybe_unused]] MPI_Comm* comm) { return 0;}
int MPI_Comm_dup (MPI_Comm comm, MPI_Comm* newcomm) { *newcomm = comm; return 0;}

int MPI_Comm_size([[maybe_unused]] MPI_Comm comm, int* size) {*size = 1; return 0;}
int MPI_Comm_rank([[maybe_unused]] MPI_Comm comm, int* rank) {*rank = 0; return 0;}

#endif
