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
  std::string mpi_filename_tail = "-collated";

  // Format the report on each task and buffer it on each task
  formatter_.execute_format(buffer, hashvec, mpi_context_);

  std::string filename = output_filename_ + mpi_filename_tail;

#ifdef USE_VERNIER_MPI_STUB
  /*
   * Rather than dummy out all the MPI calls, replace with a simple
   * file open and write when running without MPI.
   */
  std::ofstream os(filename);

  os << buffer.str();
  os.flush();
  os.close();

#else // USE_VERNIER_MPI_STUB

  int length;              // Local string length
  int max_length;          // Global maximum string length
  MPI_Datatype mpi_buffer; // Buffer as a contiguous item
  MPI_File file_handle;    // MPI File accessor
  MPI_Status status;       // Result of write
  MPI_Offset displacement; // Displacement in bytes on this rank.

  // Global maximum string size is required on every task to set up
  // the custom data type
  length = static_cast<int>(buffer.str().length());
  MPI_Allreduce(&length, &max_length, 1, MPI_INT, MPI_MAX,
                mpi_context_.get_handle());

  // Pad out with spaces
  buffer << std::string(
      static_cast<std::string::size_type>(max_length - length), ' ');

  MPI_Type_contiguous(max_length, MPI_CHAR, &mpi_buffer);
  MPI_Type_commit(&mpi_buffer);

  // Open the global output file and create a view for each task which
  // represents a unique, non-overlapping region
  MPI_File_open(mpi_context_.get_handle(), filename.c_str(),
                MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_handle);

  displacement = static_cast<MPI_Offset>(mpi_context_.get_rank() * max_length *
                                         static_cast<int>(sizeof(char)));
  MPI_File_set_view(file_handle, displacement, MPI_CHAR, mpi_buffer, "native",
                    MPI_INFO_NULL);

  // Collective write operation
  // Some evidence of memory leak risk with MPI_File_write_all.
  MPI_File_write(file_handle, buffer.str().c_str(), max_length, MPI_CHAR,
                 &status);

  // Tidy up resources
  MPI_File_close(&file_handle);
  MPI_Type_free(&mpi_buffer);

#endif // USE_VERNIER_MPI_STUB

  return;
}
