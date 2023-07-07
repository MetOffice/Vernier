/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"

/**
 * @brief  Set data members common to all Writer objects. 
 *
 */

Writer::Writer()
{
  // Pick up environment variable filename if it exists. If it's not set, a
  // suitable default is set in the data member declaration. 
  const char* env_output_filename = std::getenv("VERNIER_OUTPUT_FILENAME");
  if (env_output_filename) {output_filename_ = env_output_filename;}

  // MPI handling
  MPI_Comm_dup(MPI_COMM_WORLD, &vernier_comm_);
  MPI_Comm_rank(vernier_comm_, &my_rank_);
}
