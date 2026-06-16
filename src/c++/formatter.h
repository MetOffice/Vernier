/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   formatter.h
 * @brief  Formatter class, which contains methods for writing in different
 *         output formats.
 *
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <fstream>
#include <ostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "hashvec.h"
#include "mpi_context.h"

namespace meto {

/**
 * @brief  Formatter class. Methods write profile data.
 * @note   Different formats are coded in different class methods. A function
 *         pointer to the appropriate method is set on construction, according
 *         to the value of the environment variable PROF_OUTPUT_FORMAT.
 */

class Formatter {

private:
  std::string format_string_;
  // Format method
  void (Formatter::*format_)(std::ostream &header, std::ostream &os,
                             const hashvec_t &hashvec);

  // Individual formatter functions
  void default_output(std::ostream &header, std::ostream &os,
               const hashvec_t &hashvec);
  void drhook(std::ostream &header, std::ostream &os, const hashvec_t &hashvec);

public:
  // Constructor
  explicit Formatter();

  // Execute the format method
  void execute_format(std::ostream &header, std::ostream &os,
                      const hashvec_t &hashvec);

  [[nodiscard]] std::string  get_format_string() const { return format_string_; }
};

} // namespace meto

#endif
