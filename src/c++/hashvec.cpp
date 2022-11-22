/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "hashvec.h"

/**
 * @brief  HashVec constructor
 *
 */

HashVec::HashVec() : iomode_(std::getenv("PROF_IO_MODE")), format_(std::getenv("PROF_OUT_FORMAT")) {}

/**
 * @brief  hashvec_ getter
 *
 */

std::vector<std::pair<size_t, HashEntry>>& HashVec::get()
{
    return hashvec_;
}

/**
 * @brief  Sorts entries in the hashvec from high to low self walltime
 *
 */

void HashVec::sort()
{
    std::sort
    (
        begin(hashvec_), end(hashvec_),
        [] (auto a, auto b) { 
            return a.second.self_walltime_ > b.second.self_walltime_; 
        }
    );
}

/**
 * @brief  Makes the appropriate visitor pattern calls depending on what 
 *         format_ and iomode_ are
 * 
 * The HashVec's hashvec_ is passed down to whatever format is chosen via get()
 *
 */

void HashVec::write()
{
    if ( format_ == NULL && iomode_ == NULL )
    {
        std::make_unique<Standard>()->accept(std::make_unique<Multifile>(), get());
    }
    else throw std::runtime_error("Invalid PROF_IO_MODE choice");
}