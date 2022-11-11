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
#include <functional>
#include "hashtable.h"

/**
 * @brief  
 *
 *
 */

class Formatter {

  private:

    // Write strategy
    std::function<void(std::ofstream&, Formatter&)> strategy_;

    // Hashvec
    std::vector<std::pair<size_t,HashEntry>> hashvec_;

  public:

    // Constructor
    explicit Formatter(std::function<void(std::ofstream&, Formatter&)>);

    // Member functions
    std::vector<std::pair<size_t,HashEntry>>& get_hashvec();
    void sort_hashvec();

    // Execution method
    void executeStrategy(std::ofstream&, Formatter&);

};

struct Format {

    public:

      // Strategies
      static void Standard(std::ofstream&, Formatter&);
      static void DrHook(std::ofstream&, Formatter&);

};

#endif