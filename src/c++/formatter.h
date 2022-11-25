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
#include <functional>
#include <fstream>

#include "hashtable.h"


/**
 * @brief  Formatting class
 *
 * Any callable function can be passed to this class and ran via the execute 
 * method.
 */

class Formatter {

  private:

    // Format method
    const std::function<void(std::ofstream&, std::vector<std::pair<size_t, HashEntry>>)> format_;

  public:

    // Constructor
    explicit Formatter(std::function<void(std::ofstream&, std::vector<std::pair<size_t, HashEntry>>)> format);

    void executeFormat(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec) const; 
   
};

/**
 * @brief  Struct to store the formats of interest.
 */

struct Formats {

  public:

    // Format options
    static void standard(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec);
    static void drhook(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec);
   
};

#endif