/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   formatter.h
 * @brief  

 *
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <vector>
#include <memory>
#include <fstream>
#include "hashtable.h"


class Writer; // Forward declaration of writer


class Format {

  public:

    // Virtual destructor
    virtual ~Format() = default;

    // Members
    virtual void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;
    virtual void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;

};

class Standard : public Format {

  public:

    // Members  
    void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};

class DrHook : public Format {

  public:

    // Members
    void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};

#endif