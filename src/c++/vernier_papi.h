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
#include <string>
#include <vector>


#ifndef VERNIER_PAPI_H
#define VERNIER_PAPI_H

// The maximum number of metrics collected for an eventset. If this is
// too big, the metrics could be multiplexed.
#define VERNIER_MAX_PAPI_METRICS 5

namespace meto {


// General functions
void papi_init(int);
void papi_finalize();

// Contains the codes of the PAPI events that need to be collected.
using events_vector = std::vector<std::pair<int, std::string>>;
extern events_vector events_code;

/**
* @brief  Manage PAPI events and data.
*
*/

class PAPIContext {

private:
  bool initialized_;
  bool started_;
  int event_set_;
  int num_events_;

  // PMU (Performance Monitoring Unit) registers used by PAPI to
  // collects metrics are usually 48 bit. Thus they can overflow in a
  // normal long run. Thus PAPIContext retrive the metrics, add them
  // into "values_" array and then reset the PMU to zero.  The
  // "values_" array is 64 bit and thus very unlikely to overflow.
  long long values_[VERNIER_MAX_PAPI_METRICS];

public:
  // Constructor
  PAPIContext();

  // Init and finalize
  bool is_initialized();


  int get_num_events();

  // The following functions need to be called by each thread

  void init();
  void finalize();

  // Read the metrics. The metrics are continuesly collected like a
  // timer, they are not reset, hence "total".
  void read(long long *total_values);

};

} // namespace meto

#endif
