/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#ifndef HASHVEC_H
#define HASHVEC_H

#include "hashtable.h"

/**
 * @brief  Class containing the "hashvec", a STL vector of key-value pairs.
 *
 * The hashvec is used to sort from high to low self walltimes, since this
 * can't be done on a traditional C++ hashtable. It also serves as a singular
 * container for each thread's hashtable to go into.
 *
 */

class HashVec {

    private: 

      // The hashvec - a vector of pairs, since std::unordered_maps' cannot 
      // be sorted via std::sort 
      std::vector<std::pair<size_t,HashEntry>> hashvec_;

    public:

      // Member functions
      std::vector<std::pair<size_t,HashEntry>>& get();
      void sort();
      void write() const;

};

#endif 
