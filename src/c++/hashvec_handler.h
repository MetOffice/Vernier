/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   hashvec_handler.h
 * @brief  Container for the hashvec and IO write strategy.
 *
 */

#ifndef PROFILER_HASHVEC_HANDLER_H
#define PROFILER_HASHVEC_HANDLER_H

#include <algorithm>
#include <memory>

#include "writer.h"
#include "hashvec.h"

/**
 * @brief  HashVecHandler class
 * 
 * @details The HashVecHandler contains a hashvec vector as a data member.
 *          It wraps this vector with additional functionality to sort entries
 *          and set a writer strategy based on the PROF_IO_MODE environment
 *          variable.
 *
 */

class HashVecHandler {

  private:

    // Vector of hash entries.
    hashvec_t hashvec_;

    // Writer strategy
    std::unique_ptr<Writer> writer_strategy_;
    
  public:

    // Constructor
    HashVecHandler();

    // Member functions
    void sort();
    void write();
    void append(hashvec_t const&);
    
};

#endif

