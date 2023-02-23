/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <iostream>
#include "profiler.h"
#include <omp.h>
#include <mpi.h>

int main() {

    // Initialise MPI
    MPI_Init(NULL,NULL);
    MPI_Comm comm = MPI_COMM_WORLD;

    // Get current MPI rank
    int crank;
    MPI_Comm_rank(comm, &crank);

    // Begin OpenMP region
    #pragma omp parallel default(none) shared(std::cout,prof,crank)
    {
        // Get current OMP thread
        int cthread = omp_get_thread_num();

        // Profile a simple print statement
        auto prof_print = prof.start("print statement");
        #pragma omp critical
        {
            std::cout << "MPI rank " << crank << ", OMP thread " << cthread << std::endl;
        }  
        prof.stop(prof_print);
    }

    // Finish
    prof.write();
    MPI_Finalize();

    return 0;
}
