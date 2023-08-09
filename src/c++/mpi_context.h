/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   mpi_context.h
 *  @brief  Handles entries for each timed region.
 *
 *  In order to store region timings, one struct and one class are defined. The
 *  struct (RegionRecord) collects together information pertinent to a single
 *  profiled region, such as its name, total time and self time.
 *
 *  The HashTable class contains a hashtable to hold the hash entries (see
 *  above). The hashing algorithm is bundled with it, so that it remains an
 *  internal implementation detail only.
 *
 *  An assortment of methods handle admin. tasks, such as adding new entries.
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

    class MPIContext {

    private:

      MPI_Comm comm_handle_;
      int      comm_size_;
      int      comm_rank_;
     
    public:

        // Constructor
        MPIContext();

        // Getters
        int get_size();
        int get_rank();
        
    };

} // End meto namespace

#endif