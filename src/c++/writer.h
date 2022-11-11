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
#include <functional>
#include "hashtable.h"
#include "formatter.h"

/**
 * @brief  Writer class, which sits above hashtable and is constructed 
 *         within the profiler.
 *
 * Any callable function can be passed into the writer class constructor and
 * subsequently executed using the execute method. Different strategies can be
 * included as static member functions, such as MultiFile.
 *
 */

class Writer {

  private:

    // Strategy
    std::function<void(std::function<void(std::ofstream&, Formatter&)>, std::vector<HashTable>)> strategy_;

  public:

    // Constructor
    explicit Writer(std::function<void(std::function<void(std::ofstream&, Formatter&)>, std::vector<HashTable>)>);

    // Execution method
    void executeStrategy(std::function<void(std::ofstream&, Formatter&)>, std::vector<HashTable>);

    // Strategies 
    static void MultiFile(std::function<void(std::ofstream&, Formatter&)>, std::vector<HashTable>);

};

#endif
