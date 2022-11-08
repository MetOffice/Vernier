#ifndef HASHVEC_H
#define HASHVEC_H

#include "hashtable.h"

class HashVec {

    private: 

      // The hashvec - a vector of pairs, since std::unordered_maps' cannot 
      // be sorted via std::sort 
      std::vector<std::pair<size_t,HashEntry>> hashvec_;

    public:

      // Member functions
      std::vector<std::pair<size_t,HashEntry>>* return_mem_address();
      void sort();
      void write() const;

};

#endif 
