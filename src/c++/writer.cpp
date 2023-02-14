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
  const char* env_output_filename = std::getenv("PROF_OUTPUT_FILENAME");
  if (env_output_filename) {output_filename_ = env_output_filename;}

  // MPI handling
  MPI_Comm_dup(MPI_COMM_WORLD, &prof_comm_);
  MPI_Comm_rank(prof_comm_, &my_rank_);
}

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


