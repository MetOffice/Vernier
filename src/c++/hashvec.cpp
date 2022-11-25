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
 * The format_ and iomode_ variables are set via std::getenv, which checks for 
 * the given environment variable.
 *
 */

HashVec::HashVec() 
  : format_(std::getenv("PROF_OUT_FORMAT"))
  , iomode_(std::getenv("PROF_IO_MODE")) 
  {}

/**
 * @brief  Creates a new unique Writer pointer for a particular format and 
 *         "IO mode" - i.e. singular file output or multiple file output
 * 
 * @return std::unique_ptr<Writer>  Unique Writer pointer that hashvec_ is 
 *                                  passed to in order to write out data
 */

const std::unique_ptr<Writer> HashVec::createWriter() const
{
    std::string format = static_cast<std::string>(format_);
    std::string iomode = static_cast<std::string>(iomode_);

    // Creates format ptr first
    std::unique_ptr<Formatter> formatter;
    if (format.empty() || format == "standard") 
    {
        formatter = std::make_unique<Formatter>(Formats::standard);
    }
    else if (format == "drhook")
    {
        formatter = std::make_unique<Formatter>(Formats::drhook);
    }
    else throw std::runtime_error("Invalid format choice");


    // Uses format in creation of writer
    if (iomode.empty() || iomode == "multi")
    {
        return std::make_unique<Multi>(std::move(formatter));
    }
    else throw std::runtime_error("Invalid io mode choice");
}

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
    std::ofstream os;

    auto writer = createWriter();
    writer->write(os, hashvec_);
}