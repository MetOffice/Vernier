/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   vernier_papi.h
 *  @brief  Declare papi functionalities.
 *
 *  Contains abstractions for functions and classes that manage PAPI.
 *
 */



#ifndef VERNIER_PAPI_H
#define VERNIER_PAPI_H

// The maximum number of metrics collected for an eventset. If this is
// too big, the metrics could be multiplexed.
#define VERNIER_MAX_PAPI_METRICS 5

namespace meto {

// Functions prototypes
void papi_init(int);
void papi_finalize();


/**
* @brief  Manage PAPI events and data.
*
*/

class PAPIContext {

private:
  bool initialized_;
  bool started_;
  int event_set_;

  long long values_[VERNIER_MAX_PAPI_METRICS];


public:
  // Constructor
  PAPIContext();

  // Init and finalize
  bool is_initialized();

  // These two functions need to be called by each thread
  void init();
  void finalize();

};

} // namespace meto

#endif
