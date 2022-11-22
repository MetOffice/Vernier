#include "formatter.h"
#include "writer.h"
#include <algorithm>

#ifndef HASHVEC_H
#define HASHVEC_H

class HashVec {

    private:

      // The hashvec 
      std::vector<std::pair<size_t, HashEntry>> hashvec_;

      // Environment variables
      const char* format_;
      const char* iomode_;

    public:

      // Constructor
      HashVec();

      // Member functions
      std::vector<std::pair<size_t, HashEntry>>& get();
      void sort();
      void write();
    
};

#endif