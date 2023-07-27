/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "multi.h"

/**
 * @brief  Opens a unique file per mpi rank
 * 
 * @param[in] os  Output stream to write to
 */

void Multi::open_files(std::ofstream& os)
{

  // Append the MPI rank to the output filename.
  std::string mpi_filename_tail = "-" + std::to_string(my_rank_);
  output_filename_ += mpi_filename_tail;
    
  os.open(output_filename_);
}

/**
 * @brief  The main write method. Includes filehandling and calls formatter
 *         strategy.
 *
 * @param[in] os       The output stream to write to
 * @param[in] hashvec  The vector containing all necessary data
 */

void Multi::write(std::ofstream& os, hashvec_t hashvec) 
{
  open_files(os);
  formatter_.execute_format(os, hashvec);
  os.flush();
  os.close();
}
