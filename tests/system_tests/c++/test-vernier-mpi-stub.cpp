/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2025 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// System test to check that client code compiles with Vernier correctly when it
// includes the MPI headers directly.

#include "vernier.h"
#include <mpi.h>

int main()
{

  MPI_Init(NULL,NULL);

  meto::vernier.init();

  auto vnr_handle = meto::vernier.start("main");
  meto::vernier.stop(vnr_handle);

  meto::vernier.write();
  meto::vernier.finalize();

  MPI_Finalize();

}
