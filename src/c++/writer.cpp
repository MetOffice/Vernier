/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include <mpi.h>

Writer::Writer(std::unique_ptr<Formatter> formatter) 
  : formatter_(std::move(formatter))
  {}

std::unique_ptr<Formatter>& Writer::get_formatter()
{
  return formatter_;
}

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

Multi::Multi(std::unique_ptr<Formatter> formatter) 
  : Writer(std::move(formatter))
  {}

void Multi::write(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec)
{
  this->prep(os);
  this->get_formatter()->executeFormat(os, hashvec);
}


