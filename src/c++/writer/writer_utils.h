/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
--------------------------------------------------------------------------------
 Utility functions for use in the writer classes
\*----------------------------------------------------------------------------*/

#ifndef VERNIER_WRITER_UTILS_H
#define VERNIER_WRITER_UTILS_H

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

#include "mpi_context.h"

/**
 * @brief Prints the MPI rank information to the output stream.
 */
inline void rank_info(std::ostream &os, meto::MPIContext &mpi_context) {
  os << "\n"
     << "Task " << (mpi_context.get_rank() + 1) << " of "
     << mpi_context.get_size() << " : MPI rank ID " << mpi_context.get_rank()
     << "\n";
}

/**
 * @brief Prints the format version to the output stream.
 */
inline void format_version(std::ostream &os) {
  int major = 1;
  int minor = 0;
  os << "Format version: " << std::to_string(major) << "."
     << std::to_string(minor);
}

/**
 * @brief Prints a header to the output stream, including the output style and
 * format version.
 */
inline void header(std::ostream &os, const std::string_view style) {

  // Build the variable-width content strings
  std::string style_line = "   Output style: " + std::string(style);

  std::ostringstream ver_stream;
  format_version(ver_stream);
  std::string version_line = "   " + ver_stream.str();

  // Fixed-width line content (98 chars between the '#' borders)
  const int inner_width = 98;

  os << std::setfill('#') << std::setw(100) << ""
     << "\n"
     << "#" << std::setfill(' ') << std::setw(inner_width) << std::left
     << "   V E R N I E R"
     << "#\n"
     << "#" << std::setw(inner_width) << std::left << style_line << "#\n"
     << "#" << std::setw(inner_width) << std::left << version_line << "#\n"
     << std::setfill('#') << std::setw(100) << ""
     << "\n";
}

#endif // VERNIER_WRITER_UTILS_H
