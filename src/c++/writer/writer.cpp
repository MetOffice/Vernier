/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <string>

#include "writer.h"

/**
 * @brief  Set data members common to all Writer objects.
 * @param [in] The MPI context the writer will use.
 *
 */

meto::Writer::Writer(MPIContext const &mpi_context) {
  // Pick up environment variable filename if it exists. If it's not set, a
  // suitable default is set in the data member declaration.
  const char *env_output_filename = std::getenv("VERNIER_OUTPUT_FILENAME");
  if (env_output_filename) {
    output_filename_ = env_output_filename;
  }

  std::string tag = mpi_context.get_tag();

  // If the MPI context has a tag, append it to the output filename.
  if (tag != MPI_CONTEXT_NULL_STRING) {
    output_filename_ = output_filename_ + "-" + tag;
  }

  // MPI handling
  mpi_context_ = mpi_context;
}
