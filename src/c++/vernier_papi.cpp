/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "vernier_papi.h"
#include "error_handler.h"

#include <papi.h>
#include <pthread.h>
#include <cassert>

#include <cstdlib>
#include <sstream>

#ifdef VERNIER_PAPI_DEBUG
#include <fstream>
#include <iostream>
#include <sched.h>
#endif


// Contains the codes of the PAPI events that need to be collected.
meto::events_vector meto::events_code;

#ifdef VERNIER_PAPI_DEBUG
/**
 * @brief Returns a debug string showing the current thread and CPU placement.
 * @note  Written to std::cerr (unbuffered) so output is not lost on crash.
 *        This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */
static std::string papi_debug_str() {
  int logical_cpu = sched_getcpu();
  int physical_core = -1;
  {
    std::string path = "/sys/devices/system/cpu/cpu" +
                       std::to_string(logical_cpu) + "/topology/core_id";
    std::ifstream f(path);
    if (f) f >> physical_core;
  }
  std::ostringstream oss;
  oss << "thread=" << pthread_self()
      << " | logical_cpu=" << logical_cpu
      << " | physical_core=" << physical_core;
  return oss.str();
}

// This macro has been produced with the assistance of Met Office Github Copilot Enterprise
#define PAPI_DEBUG_LOG(msg) std::cerr << "[PAPI_DEBUG] " << msg << " | " << papi_debug_str() << "\n"
#else
#define PAPI_DEBUG_LOG(msg) do {} while(0)
#endif

/**
 * @brief Read the event strings from an env variable
 * @note  This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */
std::vector<std::string> read_events_str_from_env(const char* env_var) {
  std::vector<std::string> events_str;
  const char* env_val = std::getenv(env_var);
  if (!env_val) return events_str;

  std::stringstream ss(env_val);
  std::string token;
  while (std::getline(ss, token, ',')) {
    if (!token.empty()) {
      events_str.push_back(token);
    }
  }
  return events_str;
}

/**
 * @brief  Initialise PAPI
 *
 * @note once before any threads start
 */

void meto::papi_init(int max_threads) {

  PAPI_DEBUG_LOG("PAPI_library_init");
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    meto::error_handler(
                        "papi_init. Version mismatch or init failure.",
          EXIT_FAILURE);
  }

  // Initilize threads if there are more than one.  We use
  // pthread_self instead of omp_get_thread_num because the latter
  // value can be reused while the former is unique.  Without an
  // unique value, PAPI get confused.
  if(max_threads>1) {
    PAPI_DEBUG_LOG("PAPI_thread_init");
    retval = PAPI_thread_init(pthread_self);
    if (retval != PAPI_OK) {
      meto::error_handler(
                          "papi_init. Thread initialization failed.",
                          EXIT_FAILURE);
    }
  }

  // Read the events to collect from an environment variable
  // Ex: VERNIER_PAPI_EVENTS1=PAPI_FP_OPS,PAPI_TOT_INS
  auto events_str = read_events_str_from_env("VERNIER_PAPI_EVENTS1");
  for (const auto& event_str : events_str) {
    int code;
    PAPI_DEBUG_LOG("PAPI_event_name_to_code(" + event_str + ")");
    if (PAPI_event_name_to_code(event_str.c_str(), &code) != PAPI_OK) {
      meto::error_handler(
                          "papi_init. Failed to find the PAPI code of event: " +
                          event_str,
                          EXIT_FAILURE);
    }
    events_code.emplace_back(code,event_str);
  }

}


/**
 * @brief  Finalize PAPI
 *
 * @note once at programm end
 */

void meto::papi_finalize() {
  // This need to be cleared in case PAPIContext is initialized and finalized
  // multiple time on the same run
  events_code.clear();

  PAPI_DEBUG_LOG("PAPI_shutdown");
  PAPI_shutdown();
}

/**
* @brief  Constructor for a PAPI context.
* @details This constructor does not initialise the events.
*/

meto::PAPIContext::PAPIContext() :
  initialized_(false),
  started_(false),
  event_set_(PAPI_NULL),
  num_events_(0) {
}


/**
 * @brief Initialise PAPI context and start collecting the metrics.
 *
 * @note This need to be called inside the thread that will compute
 * the metrics.
 */

void meto::PAPIContext::init() {
  // Check that the storage is correctly null first.
  assert(event_set_ == PAPI_NULL);
  assert(initialized_ == false);
  assert(started_ == false);

  PAPI_DEBUG_LOG("PAPI_create_eventset");
  if( PAPI_create_eventset(&event_set_) != PAPI_OK ) {
    meto::error_handler(
                        "PAPIContext::init. Failed to create eventset.",
                        EXIT_FAILURE);
  }

  initialized_ = true;

  // Add the events to collect metrics and start collecting
  if (events_code.size() > 0) {

    num_events_=0;
    for (const auto& code : events_code) {
      PAPI_DEBUG_LOG("PAPI_add_event(" + code.second + ")");
      int ret_val = PAPI_add_event(event_set_, code.first) ;
      if( ret_val != PAPI_OK) {
        std::stringstream ss;
        ss << "PAPIContext::init. Failed to add event (" <<
          code.second << "): " << PAPI_strerror(ret_val);
        meto::error_handler(
                            ss.str(),
                            EXIT_FAILURE);
      }
      num_events_++;
    }

    PAPI_DEBUG_LOG("PAPI_start");
    if( PAPI_start(event_set_) != PAPI_OK ) {
      meto::error_handler(
                          "PAPIContext::init. Failed to start metric collection.",
                          EXIT_FAILURE);
    }
    started_ = true;
  }
}

/**
 * @brief  Finaliser for PAPI  context.
 *
 * @note Each thread that call init should call finalize.
 */

void meto::PAPIContext::finalize() {

  if(initialized_ && event_set_ != PAPI_NULL) {

    // Nedd to stop metrics if started values are not used after this
    // thus we can use them in this call.
    if(started_) {
      PAPI_DEBUG_LOG("PAPI_stop");
      long long values[VERNIER_MAX_PAPI_METRICS];
      if (PAPI_stop(event_set_, values) != PAPI_OK) {
        meto::error_handler(
                            "PAPIContext::finalize. Failed to stop metrics collection.",
                            EXIT_FAILURE);
      }
      started_=false;
    }

    PAPI_DEBUG_LOG("PAPI_cleanup_eventset");
    if( PAPI_cleanup_eventset(event_set_) != PAPI_OK ) {
      meto::error_handler(
                          "PAPIContext::finalize. Failed to cleanup.",
                          EXIT_FAILURE);
    }
    PAPI_DEBUG_LOG("PAPI_destroy_eventset");
    if( PAPI_destroy_eventset(&event_set_)  != PAPI_OK ) {
      meto::error_handler(
                          "PAPIContext::finalize. Failed to destroy eventset.",
                          EXIT_FAILURE);
    }
  }

  event_set_ = PAPI_NULL;
  initialized_ = false;
}



/**
 * @brief  Read the metrics.
 * @returns The total values of the metrics collected.
 *
 * @note The metrics are continuesly collected like time passed, they
 *  are not reset, hence "total". Also only the metrics of the calling
 *  thread are collected.
 */

void meto::PAPIContext::read(metrics_array& total_values) {

  assert(num_events_ <= VERNIER_MAX_PAPI_METRICS);

  // Do nothing if PAPI is not initilized or started.
  if(!initialized_ || !started_)
    return;

  PAPI_DEBUG_LOG("PAPI_read");
  if( PAPI_read(event_set_,total_values.data())  != PAPI_OK ) {
    meto::error_handler(
                        "PAPIContext::read. Failed to read eventset.",
                        EXIT_FAILURE);
  }
}
