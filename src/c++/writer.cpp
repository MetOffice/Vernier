/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include <mpi.h>

/**
 * @brief  Writer constructor
 * 
 * @param[in] formatter  A pointer to the formatter class that will replace 
 *                       Writer::formatter_
 */

Writer::Writer(std::unique_ptr<Formatter> formatter) 
  : formatter_(std::move(formatter))
  {}

/**
 * @brief  Tool via which any derived classes can access formatter_
 * 
 * @returns std::unique_ptr<Formatter>&  Returns the private formatter pointer
 */

// const std::unique_ptr<Formatter> Writer::get_formatter()
// {
//   return formatter_;
// }

/**
 * @brief  Opens a unique file per mpi rank
 * 
 * @param[in] os  Output stream to write to
 */

void Multi::prep(std::ofstream& os)
{
  // Find current MPI rank
  int current_rank;
  MPI_Comm prof_comm_ = MPI_COMM_WORLD;
  MPI_Comm_rank(prof_comm_, &current_rank);

  // For later appending onto the end of the output file name 
  std::string mpi_filename_tail = "-" + std::to_string(current_rank);

  // Pickup environment variable filename if it exists, if not use the 
  // default name of "profiler-output". In either case, include the MPI rank
  // in the name of the file.
  std::string out_filename;
  const char* env_variable = std::getenv("PROF_OUTFILE");
  if (env_variable != NULL)
  {
      out_filename = static_cast<std::string>(env_variable) + mpi_filename_tail;
  }
  else
  {
      out_filename = "profiler-output" + mpi_filename_tail;
  }
    
  os.open(out_filename);
}

/**
 * @brief  Multiple-file-output "Multi" class constructor
 * 
 * @param[in] formatter  An input formatter pointer that will be used to call
 *                       Writer's constructor 
 */

Multi::Multi(std::unique_ptr<Formatter> formatter) 
  : Writer(std::move(formatter))
  {}

/**
 * @brief  The main write method, combines prep() with formatter_'s format,
 *         before then flushing and closing the output stream.
 * 
 * @param[in] os       The output stream to write to
 * @param[in] hashvec  The vector containing all necessary data
 */

void Multi::write(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec) 
{
  prep(os);
  this->formatter_->executeFormat(os, hashvec);
  os.flush();
  os.close();
}


