
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



