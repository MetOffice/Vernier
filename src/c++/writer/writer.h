/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   writer.h
 * @brief  Writer base strategy class.
 * 
 */

#ifndef VERNIER_WRITER_H
#define VERNIER_WRITER_H

#include "../mpi_context.h"
#include "../formatter.h"

namespace meto
{

/**
 * @brief   Abstract Writer strategy class.
 * @details Specific implementations of this class override the 'write' function
 *          to produce different behaviour.
 * 
 */

class Writer {

  protected:

    // Formatter strategy
    Formatter formatter_;

    // Default filename 
    std::string output_filename_ = "vernier-output";

    // MPI context
    MPIContext mpi_context_;

  public:

    explicit Writer(MPIContext const&);
    virtual ~Writer() = default;

    // Pure virtual write method
    virtual void write(std::ofstream& os, hashvec_t) = 0;

};

} // End meto namespace

#endif
