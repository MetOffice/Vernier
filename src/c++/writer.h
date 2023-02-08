/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   writer.h
 * @brief  Writer strategy classes.
 *
 */

#ifndef WRITER_H
#define WRITER_H

#include <mpi.h>

#include "hashvec.h"
#include "formatter.h"

/**
 * @brief   Abstract Writer strategy class.
 * @details Specific implementations of this class override the `write` function
 *          to produce different behaviour.
 *
 */

class Writer {

  protected:

    // Formatter strategy
    Formatter formatter_;

    // Default filename 
    std::string output_filename_ = "profiler-output";

    // MPI handling
    int my_rank_;
    MPI_Comm prof_comm_;

  public:

    explicit Writer();
    virtual ~Writer() = default;

    // Pure virtual write method
    virtual void write(std::ofstream& os, hashvec_t) = 0;

};

/**
 * @brief   Multiple-file output strategy
 * @details Creates one file per MPI rank.
 *
 */

class Multi : public Writer {

  private:

    // Method 
    void open_files(std::ofstream& os);

  public:

    // Implementation of pure virtual function.
    void write(std::ofstream& os, hashvec_t) override;

};

#endif
