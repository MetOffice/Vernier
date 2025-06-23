/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   multi.h
 * @brief  Multi class, derived from Writer.
 *
 */

#ifndef VERNIER_MULTI_H
#define VERNIER_MULTI_H

#include "../mpi_context.h"
#include "writer.h"

namespace meto {

/**
 * @brief   Multiple-file output strategy
 * @details Creates one file per MPI rank.
 *
 */

class Multi : public Writer {

private:
  // Per-file output stream
  std::ofstream os;
  // Method
  void open_files();

public:
  // Constructor
  Multi(MPIContext const &);

  // Implementation of pure virtual function.
  void write(hashvec_t) override;
};

} // namespace meto

#endif
