/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2025 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   single.h
 * @brief  SingleFile class, derived from Writer.
 *
 */

#ifndef VERNIER_SINGLE_H
#define VERNIER_SINGLE_H

#include <ostream>
#include <sstream>

#include "../mpi_context.h"
#include "writer.h"

namespace meto {

  /**
   * @brief   Single parallel output file
   * @details Creates a single file for all ranks using MPI IO
   */

  class SingleFile : public Writer {
  public:
    SingleFile(MPIContext const &);
    void write(hashvec_t) override;
  };

} // namespace meto

#endif // VERNIER_SINGLE_H
