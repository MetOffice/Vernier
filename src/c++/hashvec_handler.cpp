/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "hashvec_handler.h"
#include "error_handler.h"

/**
 * @brief  HashVecHandler constructor
 *
 * @note  Allocates the writer strategy based on the PROF_IO_MODE environment
 *        variable. 
 *
 */

meto::HashVecHandler::HashVecHandler() 
{

    // Default the IO mode to one file per MPI rank.
    std::string io_mode = "multi";

    // Read environment variable.
    char const* env_io_mode = std::getenv("VERNIER_OUTPUT_MODE");
    if (env_io_mode) {  io_mode = env_io_mode; }

    // Allocate writer to be of required type.
    if (io_mode == "multi")
    {
        writer_strategy_ = std::make_unique<Multi>();
    }
    else {
        error_handler("Invalid IO mode choice", EXIT_FAILURE);
    }  
}

/**
 * @brief  Appends a hashvec vector to the HashVecHandler data member.
 * @param  [in]  append_hashvec  The vector to append.
 *
 */

void meto::HashVecHandler::append(hashvec_t const& append_hashvec)
{
  hashvec_.insert(hashvec_.end(), append_hashvec.begin(), append_hashvec.end());
}

/**
 * @brief  Sorts hash entries from high to low self walltime.
 *
 */

void meto::HashVecHandler::sort()
{
    std::sort
    (
        begin(hashvec_), end(hashvec_),
        [] (auto a, auto b) { 
            return a.self_walltime_ > b.self_walltime_; 
        }
    );
}

/**
 * @brief  Calls the writer strategy.
 *
 */

void meto::HashVecHandler::write()
{
    std::ofstream os;
    writer_strategy_->write(os, hashvec_);
}

