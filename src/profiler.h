/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <string>
#include <string_view>
#include <cassert>
#include <list>
#include <iterator>
#include <utility>
#include <vector>

#include "omp.h"

#include "hashtable.h"

class Profiler
{
  private: 

    // Data members
    int max_threads_;
    std::vector<HashTable>                               thread_hashtables_;
    std::vector<std::vector<std::pair<size_t,double>>>   thread_traceback_;

    // Type definitions for vector array indexing.
    typedef std::vector<HashTable>::size_type                        hashtable_iterator_t_;
    typedef std::vector<std::vector<std::pair<size_t,double>>>::size_type pair_iterator_t_;

  public:

    // Constructors
    Profiler();

    // Member functions
    size_t start(std::string_view);
    void   stop (size_t const);
    void   write();

};

// Declare global profiler
inline Profiler prof;

#endif
