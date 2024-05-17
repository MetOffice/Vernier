/* -----------------------------------------------------------------------------
 * Crown copyright 2024 Met Office. All rights reserved.
 * The file LICENCE, distributed with this code, contains details of the terms
 * under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include "vernier_mpi.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  MPI_Init(&argc, &argv);

  auto result = RUN_ALL_TESTS();

  MPI_Finalize();

  return result;
}

