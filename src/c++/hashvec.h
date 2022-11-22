/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   hashvec.h
 * @brief  Contains the hashvec and decides on format/IO mode.
 *
 * The hashvec is a vector of pairs that acts as an alternative hashtable for 
 * sorting via self-walltime. The HashVec constructor grabs the appropriate
 * format and IO strategy environment variables then uses the writer/formatter 
 * visitor pattern.
 *
 */

#ifndef HASHVEC_H
#define HASHVEC_H

#include "formatter.h"
#include "writer.h"
#include <algorithm>

/**
 * @brief  Base writer class
 *
 * Essentially a visitor class that all IO modes will derive from. 
 */

class HashVec {

    private:

      // The hashvec 
      std::vector<std::pair<size_t, HashEntry>> hashvec_;

      // Environment variables
      const char* iomode_;
      const char* format_;
      
    public:

      // Constructor
      HashVec();

      // Member functions
      std::vector<std::pair<size_t, HashEntry>>& get();
      void sort();
      void write();
    
};

#endif