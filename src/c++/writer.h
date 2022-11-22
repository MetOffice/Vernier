/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   writer.h
 * @brief  Contains the writer class, which handles IO.
 *
 * Implements a strategy pattern that favours using functions over 
 * single-method objects, hence reducing the number of classes required. 
 *
 */

#ifndef WRITER_H
#define WRITER_H

#include <vector>
#include <memory>
#include <fstream>
#include "hashtable.h"


// Forward declaration of formats
class Standard; 
class DrHook;


/**
 * @brief  Base writer class
 *
 * Essentially a visitor class that all IO modes will derive from. 
 */

class Writer {

  public:

    // Virtual destructor
    virtual ~Writer() = default;

    // Members - any writer can "visit" any format
    virtual void VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;
    virtual void VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;

};

/**
 * @brief  Class for the multiple-file output option
 */

class Multifile : public Writer {

  private:

    // Output stream
    std::ofstream output_stream;

    // Private init/finalise methods
    void openfile();
    void closefile();

  public:

    // Override the visit methods
    void VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};


#endif
