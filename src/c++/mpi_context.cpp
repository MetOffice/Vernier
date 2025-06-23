/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <cassert>
#include <stdexcept>

#include <fstream>
#include <sstream>

#include "error_handler.h"
#include "mpi_context.h"

/**
 * @brief  Constructor for an MPI context.
 * @details This constructor does not initialise MPI.
 */

meto::MPIContext::MPIContext() { reset(); }

/**
 * @brief  Resets data members to sensible null values.
 *
 */

void meto::MPIContext::reset() {
  comm_handle_ = MPI_COMM_NULL;
  comm_rank_ = -1;
  comm_size_ = -1;
  initialized_ = false;
  tag_ = MPI_CONTEXT_NULL_STRING;
}

/**
 * @brief  Initialise a Vernier MPI context.
 * @param [in] comm_client_handle  The communicator to duplicate.
 * @param [in] tag  Identifying tag.
 * @note  Duplicates the input MPI communicator.
 */

void meto::MPIContext::init(MPI_Comm client_comm_handle, std::string_view tag) {

  // Check that the storage is correctly null first.
  assert(comm_handle_ == MPI_COMM_NULL);
  assert(comm_rank_ == -1);
  assert(comm_size_ == -1);

  int mpi_is_initialised;
  MPI_Initialized(&mpi_is_initialised);

  if (mpi_is_initialised) {

    // If MPI is initialised, then the passed communicator should not be null.
    if (client_comm_handle == MPI_COMM_NULL) {
      meto::error_handler(
          "MPIContext::init. MPI initialized, but null communicator passed.",
          EXIT_FAILURE);
    }

    MPI_Comm_dup(client_comm_handle, &comm_handle_);
    MPI_Comm_rank(comm_handle_, &comm_rank_);
    MPI_Comm_size(comm_handle_, &comm_size_);
  } else {
    meto::error_handler("MPIContext::init. MPI not initialized.", EXIT_FAILURE);
  }

  tag_ = tag;

  initialized_ = true;

  // Assertions
  assert(comm_handle_ != MPI_COMM_NULL);
  assert(comm_handle_ != MPI_COMM_WORLD);
  assert(comm_rank_ >= 0);
  assert(comm_size_ >= 0);
}

/**
 * @brief  Returns true if the Vernier MPI context is initialised.
 * @returns  Boolean initialisation status.
 */

bool meto::MPIContext::is_initialized() {
  // Returning local_initialized ought to be sufficient. Belt and braces.
  bool local_initialized = initialized_;
  return local_initialized;
}

/**
 * @brief  Finaliser for a Vernier MPI context.
 * @note   Since the constructor duplicated an MPI communicator, creating a new
 *         communicator handle in the process, this destructor needs to free
 * that communicator handle.
 */

void meto::MPIContext::finalize() {
  if (comm_handle_ != MPI_COMM_WORLD && comm_handle_ != MPI_COMM_NULL) {
    MPI_Comm_free(&comm_handle_);
  }

  reset();
}

/**
 * @brief Gets the MPI rank from an MPIContext object.
 * @returns The MPI rank.
 */

int meto::MPIContext::get_rank() { return comm_rank_; }

/**
 * @brief Gets the size of the MPI communicator from an MPIContext object.
 * @returns The MPI communicator size.
 */

int meto::MPIContext::get_size() { return comm_size_; }

/**
 * @brief Write a string from each task to a global file
 */
void meto::MPIContext::write_global_file(std::string filename,
                                         std::ostringstream &buffer) {
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

  // Global maximum string size is required on every task to set up
  // the custom data type
  length = static_cast<int>(buffer.str().length());
  MPI_Allreduce(&length, &max_length, 1, MPI_INT, MPI_MAX, comm_handle_);

  MPI_Type_contiguous(max_length, MPI_CHAR, &mpi_buffer);
  MPI_Type_commit(&mpi_buffer);

  // Open the global output file and create a view for each task which
  // represents a unique, non-overlapping region
  MPI_File_open(comm_handle_, filename.c_str(),
                MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_handle);

  MPI_File_set_view(file_handle, max_length * comm_rank_, MPI_CHAR, mpi_buffer,
                    "native", MPI_INFO_NULL);

  // Collective write operation
  MPI_File_write_all(file_handle, buffer.str().c_str(), max_length, MPI_CHAR,
                     &status);

  // Tidy up resources
  MPI_File_close(&file_handle);
  MPI_Type_free(&mpi_buffer);

#endif // USE_VERNIER_MPI_STUB

  return;
}

/**
 * @brief Gets the identifying tag.
 * @returns The tag, as a string.
 */

std::string meto::MPIContext::get_tag() const { return tag_; }
