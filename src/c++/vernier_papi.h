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

#include <array>
#include <string>
#include <vector>

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


// The type of an array that contains the metrics
using metrics_array  = std::array<long long,VERNIER_MAX_PAPI_METRICS>;
// The vector that can contains multiple metrics_array.
using metrics_vector = std::vector<metrics_array>;

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

public:
  // Constructor
  PAPIContext();


  bool is_initialized() const {  return initialized_; }
  int get_num_events() const {  return num_events_; }

  // The following functions need to be called by each thread

  void init();
  void finalize();

  // Read the metrics. The metrics are continuesly collected like a
  // timer, they are not reset, hence "total".
  void read(metrics_array& total_values);

};

} // namespace meto

#endif
