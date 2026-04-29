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

#ifdef USE_PAPI

// Contains the codes of the PAPI events that need to be collected.
using events_vector_t = std::vector<std::pair<int, std::string>>;
extern events_vector_t events_code;

// General functions
void papi_init(int);
void papi_finalize();

// Probe whether every event listed in VERNIER_PAPI_EVENTS1 can be added to a
// PAPI event set on the current hardware.  Returns true when all events are
// available, false when any event is missing or cannot be counted.  Never
// calls error_handler; safe to call before vernier.init().
// This routine is used for unit and system testing.
bool papi_events_probe();

// The type of an array that contains the metrics
using metrics_array_t = std::array<long long, VERNIER_MAX_PAPI_METRICS>;
// The vector that can contain multiple metrics_array.
using metrics_vector_t = std::vector<metrics_array_t>;

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

  bool is_initialized() const { return initialized_; }
  int get_num_events() const { return num_events_; }

  // The following functions need to be called by each thread

  void init();
  void finalize();

  // Read the metrics. The metrics are continuously collected like a
  // timer, they are not reset, hence "total".
  void read(metrics_array_t &total_values);
};

#else

// Dummy classes, types and functions due to PAPI not being available.

struct events_vector_t {
  using value_type = std::pair<int, std::string>;

  constexpr bool empty() const { return true; }
  constexpr std::size_t size() const { return 0; }

  value_type operator[](std::size_t) const { return value_type{}; }
};

extern events_vector_t events_code;

using metrics_array_t = std::array<long long, 0>;
using metrics_vector_t = std::vector<metrics_array_t>;

inline void papi_init(int) {}
inline void papi_finalize() {}
inline bool papi_events_probe() { return false; }

struct PAPIContext {

  PAPIContext() = default;

  constexpr bool is_initialized() const { return false; }
  constexpr int get_num_events() const { return 0; }
  constexpr void init() {}
  constexpr void finalize() {}
  constexpr void read([[maybe_unused]] metrics_array_t &total_values){};
};

#endif

} // namespace meto
#endif
