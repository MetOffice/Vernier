/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "vernier_papi.h"
#include "error_handler.h"

#include <cassert>
#include <papi.h>
#include <pthread.h>

#include <cstdlib>
#include <sstream>

#ifdef VERNIER_PAPI_DEBUG
#include <fstream>
#include <iostream>
#include <mutex>
#include <sched.h>
#endif

// Contains the codes of the PAPI events that need to be collected.
meto::events_vector meto::events_code;

// PAPI library should be initialized only once even if vernier is
// initialized and finalized multiple times.
static bool papi_library_initialized_ = false;

// Thread support must be initialized separately.
static bool papi_thread_initialized_ = false;

#ifdef VERNIER_PAPI_DEBUG
/**
 * @brief Returns a debug string showing the current thread and CPU placement.
 * @note  This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */
static std::string papi_debug_str() {
  int logical_cpu = sched_getcpu();
  int physical_core = -1;
  {
    std::string path = "/sys/devices/system/cpu/cpu" +
                       std::to_string(logical_cpu) + "/topology/core_id";
    std::ifstream f(path);
    if (f)
      f >> physical_core;
  }
  std::ostringstream oss;
  oss << "thread=" << pthread_self() << " | logical_cpu=" << logical_cpu
      << " | physical_core=" << physical_core;
  return oss.str();
}

/**
 * @brief Print the debug log one thread at a time.
 * @note  Written to std::cerr (unbuffered) so output is not lost on crash.
 *        This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */

static void papi_debug_log_impl(const std::string &msg) {
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> lock(log_mutex);
  std::cerr << "[PAPI_DEBUG] " << msg << " | " << papi_debug_str() << "\n";
}

// This macro has been produced with the assistance of Met Office Github Copilot
// Enterprise
#define PAPI_DEBUG_LOG(msg) papi_debug_log_impl(msg)
#else
#define PAPI_DEBUG_LOG(msg)                                                    \
  do {                                                                         \
  } while (0)
#endif

/**
 * @brief Read the event strings from an env variable
 * @note  This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */
static std::vector<std::string> read_events_str_from_env(const char *env_var) {
  std::vector<std::string> events_str;
  const char *env_val = std::getenv(env_var);
  if (!env_val)
    return events_str;

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
 * @brief Probe whether every event in VERNIER_PAPI_EVENTS1 is
 *        countable on the current hardware. This routine is used for
 *        unit and system testing.
 *
 * @details Initialises the PAPI library (idempotent), then attempts to add
 *          each requested event to a temporary event set.  The event set is
 *          always destroyed before the function returns.  No error_handler
 *          call is made; the function returns false on any failure so that
 *          callers can issue GTEST_SKIP() before invoking vernier.init().
 *
 * @return true  All requested events are available (or no events requested).
 * @return false PAPI cannot be initialised, or at least one event cannot be
 *               added to an event set on this hardware.
 * @note  This function has been produced with the assistance of
 *        Met Office Github Copilot Enterprise
 */
bool meto::papi_events_probe() {
  // Initialise the PAPI library if it has not been done yet.
  if (!papi_library_initialized_) {
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
      return false;
    }
    papi_library_initialized_ = true;
  }

  auto events_str = read_events_str_from_env("VERNIER_PAPI_EVENTS1");
  if (events_str.empty()) {
    return true; // nothing requested — counts as available
  }

  int es = PAPI_NULL;
  if (PAPI_create_eventset(&es) != PAPI_OK) {
    return false;
  }

  bool all_ok = true;
  for (const auto &name : events_str) {
    int code = 0;
    if (PAPI_event_name_to_code(name.c_str(), &code) != PAPI_OK) {
      all_ok = false;
      break;
    }
    if (PAPI_add_event(es, code) != PAPI_OK) {
      all_ok = false;
      break;
    }
  }

  PAPI_cleanup_eventset(es);
  PAPI_destroy_eventset(&es);
  return all_ok;
}

/**
 * @brief  Initialise PAPI
 *
 * @note once before any threads start
 */

void meto::papi_init(int max_threads) {

  if (!papi_library_initialized_) {

    PAPI_DEBUG_LOG("PAPI_library_init");
    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
      meto::error_handler("papi_init. Version mismatch or init failure.",
                          EXIT_FAILURE);
    }

    papi_library_initialized_ = true;
  }

  // Thread support must be initialized separately from the library guard so
  // that a prior call to papi_events_probe() (which sets
  // papi_library_initialized_ without knowing the thread count) does not
  // prevent PAPI_thread_init from being called here.
  //
  // We use pthread_self instead of omp_get_thread_num because the latter
  // value can be reused while the former is unique.  Without a unique value,
  // PAPI gets confused.  However, if the code spawns threads without using
  // OMP, the behaviour is undefined.
  if (!papi_thread_initialized_ && max_threads > 1) {
    PAPI_DEBUG_LOG("PAPI_thread_init");
    int retval = PAPI_thread_init(pthread_self);
    if (retval != PAPI_OK) {
      meto::error_handler("papi_init. Thread initialization failed.",
                          EXIT_FAILURE);
    }
    papi_thread_initialized_ = true;
  }

  // Read the events to collect from an environment variable
  // Ex: VERNIER_PAPI_EVENTS1=PAPI_FP_OPS,PAPI_TOT_INS
  auto events_str = read_events_str_from_env("VERNIER_PAPI_EVENTS1");
  if (events_str.size() > VERNIER_MAX_PAPI_METRICS) {
    meto::error_handler("papi_init. VERNIER_PAPI_EVENTS has too many events: " +
                            std::to_string(events_str.size()),
                        EXIT_FAILURE);
  }
  for (const auto &event_str : events_str) {
    int code;
    PAPI_DEBUG_LOG("PAPI_event_name_to_code(" + event_str + ")");
    if (PAPI_event_name_to_code(event_str.c_str(), &code) != PAPI_OK) {
      meto::error_handler("papi_init. Failed to find the PAPI code of event: " +
                              event_str,
                          EXIT_FAILURE);
    }
    events_code.emplace_back(code, event_str);
  }
}

/**
 * @brief  Finalize PAPI
 *
 * @note once at program end
 */

void meto::papi_finalize() {
  // This needs to be cleared in case PAPIContext is initialized and finalized
  // multiple times on the same run
  events_code.clear();
}

/**
 * @brief  Constructor for a PAPI context.
 * @details This constructor does not initialise the events.
 */

meto::PAPIContext::PAPIContext()
    : initialized_(false), started_(false), event_set_(PAPI_NULL),
      num_events_(0) {}

/**
 * @brief Initialise PAPI context and start collecting the metrics.
 *
 * @note This needs to be called inside the thread that will compute
 * the metrics.
 */

void meto::PAPIContext::init() {
  // Check that the storage is correctly null first.
  assert(event_set_ == PAPI_NULL);
  assert(initialized_ == false);
  assert(started_ == false);

  // Register this thread with PAPI before touching any event set.
  // PAPI_thread_init() registers the thread-ID function in the calling
  // (main) thread only; every OMP worker thread must call
  // PAPI_register_thread() itself before using PAPI.  The call is a
  // no-op when the thread is already known to PAPI.
  PAPI_DEBUG_LOG("PAPI_register_thread");
  PAPI_register_thread();

  PAPI_DEBUG_LOG("PAPI_create_eventset");
  if (PAPI_create_eventset(&event_set_) != PAPI_OK) {
    meto::error_handler("PAPIContext::init. Failed to create eventset.",
                        EXIT_FAILURE);
  }

  initialized_ = true;

  // Add the events to collect metrics and start collecting
  if (!events_code.empty()) {

    num_events_ = 0;
    for (const auto &code : events_code) {
      PAPI_DEBUG_LOG("PAPI_add_event(" + code.second + ")");
      int ret_val = PAPI_add_event(event_set_, code.first);
      if (ret_val != PAPI_OK) {
        std::stringstream ss;
        ss << "PAPIContext::init. Failed to add event (" << code.second
           << "): " << PAPI_strerror(ret_val);
        meto::error_handler(ss.str(), EXIT_FAILURE);
      }
      num_events_++;
    }

    PAPI_DEBUG_LOG("PAPI_start");
    if (PAPI_start(event_set_) != PAPI_OK) {
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
 * @note Each thread that calls init should call finalize.
 */

void meto::PAPIContext::finalize() {

  if (initialized_) {

    if (event_set_ != PAPI_NULL) {

      // Need to stop metrics if started; values are not used after this,
      // thus we can use them in this call.
      if (started_) {
        PAPI_DEBUG_LOG("PAPI_stop");
        long long values[VERNIER_MAX_PAPI_METRICS];
        if (PAPI_stop(event_set_, values) != PAPI_OK) {
          meto::error_handler(
              "PAPIContext::finalize. Failed to stop metrics collection.",
              EXIT_FAILURE);
        }
        started_ = false;
      }

      PAPI_DEBUG_LOG("PAPI_cleanup_eventset");
      if (PAPI_cleanup_eventset(event_set_) != PAPI_OK) {
        meto::error_handler("PAPIContext::finalize. Failed to cleanup.",
                            EXIT_FAILURE);
      }
      PAPI_DEBUG_LOG("PAPI_destroy_eventset");
      if (PAPI_destroy_eventset(&event_set_) != PAPI_OK) {
        meto::error_handler(
            "PAPIContext::finalize. Failed to destroy eventset.", EXIT_FAILURE);
      }
    }

    event_set_ = PAPI_NULL;
    initialized_ = false;

    // Deregister this thread from PAPI to release per-thread resources.
    // Placed inside the initialized_ guard so that it is only called when
    // PAPI_register_thread() was called in init().
    PAPI_DEBUG_LOG("PAPI_unregister_thread");
    PAPI_unregister_thread();
  }
}

/**
 * @brief  Read the metrics.
 * @returns The total values of the metrics collected.
 *
 * @note The metrics are continuously collected like time passed, they
 *  are not reset, hence "total". Also only the metrics of the calling
 *  thread are collected.
 */

void meto::PAPIContext::read(metrics_array &total_values) {

  assert(num_events_ <= VERNIER_MAX_PAPI_METRICS);

  // Do nothing if PAPI is not initialized or started.
  if (!initialized_ || !started_)
    return;

  PAPI_DEBUG_LOG("PAPI_read");
  if (PAPI_read(event_set_, total_values.data()) != PAPI_OK) {
    meto::error_handler("PAPIContext::read. Failed to read eventset.",
                        EXIT_FAILURE);
  }
}
