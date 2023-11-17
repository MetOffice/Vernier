

#include "vernier_mpi.h"

#ifndef USE_MPI

int MPI_Init([[maybe_unused]] int*, [[maybe_unused]] char***){return 0;}
int MPI_Finalize(void){return 0;}

int MPI_Comm_f2c(MPI_Fint comm) { return static_cast<MPI_Comm>(comm); }
int MPI_Initialized(int* value) { *value=1; return 0; }
int MPI_Comm_free(MPI_Comm* comm) { return 0;}
int MPI_Comm_dup (MPI_Comm comm, MPI_Comm* newcomm) { *newcomm = comm; return 0;}

int MPI_Comm_size(MPI_Comm comm, int* size) {*size = 1; return 0;}
int MPI_Comm_rank(MPI_Comm comm, int* rank) {*rank = 0; return 0;}

#endif
