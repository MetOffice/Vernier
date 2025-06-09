/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2025 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "single.h"

/**
 * @brief  Construct a new single file writer.
 * @param[in] mpi_context  The MPI context the writer will use.
 */

meto::SingleFile::SingleFile(MPIContext const &mpi_context)
    : meto::SingleFile::Writer(mpi_context) {}

/**
 * @brief  The main write method.
 *
 * @param[in] hashvec  The vector containing all necessary data
 */
void meto::SingleFile::write(hashvec_t hashvec) {
  /* This is a complete cheat for now: ignore the ofstream and do
   * everything through MPI IO.
   */
  std::ostringstream buffer; // Formatted output
  std::string mpi_filename_tail = "-global";

  // Format the report on each task and buffer it on each task
  formatter_.execute_format(buffer, hashvec);

  mpi_context_.write_global_file(output_filename_ + mpi_filename_tail, buffer);
}
