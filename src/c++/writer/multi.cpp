/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "multi.h"

/**
 * @brief  Construct a new Multi writer.
 * @param[in] mpi_context  The MPI context the writer will use.
 */

meto::Multi::Multi(MPIContext const &mpi_context)
    : meto::Multi::Writer(mpi_context) {}

/**
 * @brief  Opens a unique file per mpi rank
 */

void meto::Multi::open_files() {

  // Append the MPI rank to the output filename.
  std::string mpi_filename_tail = "-" + std::to_string(mpi_context_.get_rank());
  output_filename_ += mpi_filename_tail;

  os.open(output_filename_);
}

/**
 * @brief  The main write method. Includes filehandling and calls formatter
 *         strategy.
 *
 * @param[in] hashvec  The vector containing all necessary data
 */

void meto::Multi::write(hashvec_t hashvec) {
  open_files();
  formatter_.execute_format(os, hashvec);
  os.flush();
  os.close();
}
