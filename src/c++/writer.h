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


class Standard; // Forward declaration of formats
class DrHook;


class Writer {

  protected:

    //
    std::ofstream output_stream;

  public:

    // Virtual destructor
    virtual ~Writer() = default;

    // Members
    virtual void VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;
    virtual void VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t,HashEntry>> hashvec) = 0;

};

class Singlefile : public Writer {

  private:

    //
    void write();

  public:

    //
    void VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};

class Multifile : public Writer {

  private:

    //
    void write();

  public:

    //
    void VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t,HashEntry>> hashvec) override;
    void VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t,HashEntry>> hashvec) override;

};


#endif
