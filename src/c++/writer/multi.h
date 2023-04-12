/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   multi.h
 * @brief  Multi class, derived from Writer.
 * 
 */

#ifndef MULTI_H
#define MULTI_H

#include "writer.h"

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
