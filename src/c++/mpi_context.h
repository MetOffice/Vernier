/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
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

/**
 * @file   mpi_context.h
 * @brief  Defines an MPI context class, reducing the code-bases requirement for
 *         MPI calls.
 */

#ifndef VERNIER_MPI_CONTEXT_H
#define VERNIER_MPI_CONTEXT_H

#include <unordered_map>
#include <mpi.h>

#include "hashvec.h"
#include "vernier_gettime.h"

namespace meto
{

/**
 * @brief  Provides a wrapper for MPI functionality.
 */

    class MPIContext {

    private:

      MPI_Comm comm_handle_ = MPI_COMM_NULL;
      int      comm_size_   = -1;
      int      comm_rank_   = -1;
     
    public:

        // Constructor
        MPIContext(MPI_Comm client_comm_handle = MPI_COMM_NULL);
        ~MPIContext();

        // Getters
        int get_size();
        int get_rank();
        
    };

} // End meto namespace

#endif