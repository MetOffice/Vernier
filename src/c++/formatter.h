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

#ifdef _OPENMP
  #include <omp.h>
#endif

#include "hashvec.h"

namespace meto
{

/**
 * @brief  Formatter class. Methods write profile data.
 * @note   Different formats are coded in different class methods. A function
 *         pointer to the appropriate method is set on construction, according
 *         to the value of the environment variable PROF_OUTPUT_FORMAT.
 */

class Formatter {

  private:

    // Format method
    void (Formatter::*format_)(std::ofstream&, hashvec_t);

    // Individual formatter functions
    void  threads(std::ofstream& os, hashvec_t);
    void  drhook (std::ofstream& os, hashvec_t);

  public:

    // Constructor
    explicit Formatter();

    // Execute the format method
    void execute_format(std::ofstream& os, hashvec_t);
   
};

} // End meto namespace

#endif

