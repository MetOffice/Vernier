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
 * @brief  HashVec class
 * 
 * The hashvec is a vector of pairs containing information from one or more 
 * hashtables, since data in unordered_map's cannot be sorted. This class wraps
 * the hashvec container with additional functionality that lets it pick a 
 * format/iomode and create a pointer to the writer class.
 */

class HashVec {

    private:

      // The hashvec 
      std::vector<std::pair<size_t, HashEntry>> hashvec_;

      // Environment variables
      const char* format_;
      const char* iomode_;

      // Writer creation function
      std::unique_ptr<Writer> createWriter();
      
    public:

      // Constructor
      HashVec();

      // Member functions
      std::vector<std::pair<size_t, HashEntry>>& get();
      void sort();
      void write();
    
};

#endif