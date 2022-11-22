/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   formatter.h
 * @brief  Contains base format class and all derived classes.
 *
 *
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <vector>
#include <memory>
#include <fstream>
#include "hashtable.h"


// Forward declaration of writer
class Writer; 


/**
 * @brief  Base format class
 *
 * Contains virtual write method and "accept" method for accepting visitors.
 */

class Format {

  public:

    // Virtual destructor
    virtual ~Format() = default;

    // Members
    virtual void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;
    virtual void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;

};

/**
 * @brief  Standard format
 *
 * Overloads the virtual write method in order to write an input hashvec out in
 * the profiler's standard format
 */

class Standard : public Format {

  public:

    // Members  
    void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};

/**
 * @brief  DrHook format
 *
 * Overloads the virtual write method in order to write an input hashvec out in
 * a drhook-style format
 */

class DrHook : public Format {

  public:

    // Members
    void accept(std::unique_ptr<Writer> writer, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void write(std::ofstream& output_stream, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};

#endif