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
#include "formatter.h"

/**
 * @brief  Writer class
 *
 * Abstract class that stores a pointer to the formatter. Both the formatter
 * and the virtual write method can be changed by any derived classes.
 */

class Writer {

  private:

    // Ptr to formatting class
    std::unique_ptr<Formatter> formatter_;

  protected:

    // The constructor, this is the only way in which the formatter pointer
    // above can be changed. It will be inherited by any derived classes.
    explicit Writer(std::unique_ptr<Formatter> formatter);

    std::unique_ptr<Formatter>& get_formatter();

  public:

    virtual ~Writer() = default;
    Writer() = delete;

    // Virtual write method
    virtual void write(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec) = 0;

};

/**
 * @brief  Class for multiple-file output
 */

class Multi : public Writer {

  private:

    // Method 
    void prep(std::ofstream& os);

  public:

    ~Multi() override = default;
    explicit Multi(std::unique_ptr<Formatter> formatter);

    void write(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec) override;

};

#endif
