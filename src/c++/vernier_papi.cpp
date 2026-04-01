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

/**
 * @brief  Initialise PAPI
 *
 * @note once before any threads start
 */

void meto::papi_init(int max_threads) {

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
    retval = PAPI_thread_init(pthread_self);
    if (retval != PAPI_OK) {
      meto::error_handler(
                          "papi_init. Thread initialization failed.",
                          EXIT_FAILURE);
    }
  }
}


/**
 * @brief  Finalize PAPI
 *
 * @note once at programm end
 */

void meto::papi_finalize() {

  PAPI_shutdown();
}

/**
* @brief  Constructor for a PAPI context.
* @details This constructor does not initialise the events.
*/

meto::PAPIContext::PAPIContext() :
  initialized_(false),
  event_set_(PAPI_NULL) {
}


/**
 * @brief  Returns true if the Vernier PAPI context is initialised.
 * @returns  Boolean initialisation status.
 */

bool meto::PAPIContext::is_initialized() {
  // Returning local_initialized ought to be sufficient. Belt and braces.
  bool local_initialized = initialized_;
  return local_initialized;
}


/**
 * @brief  Initialise PAPI context.
 8
 */

void meto::PAPIContext::init() {

  // Check that the storage is correctly null first.
  assert(event_set_ == PAPI_NULL);
  assert(initialized_ == false);

  if( PAPI_create_eventset(&event_set_) != PAPI_OK ) {
    meto::error_handler(
                        "PAPIContext::init. Create eventset failed.",
                        EXIT_FAILURE);
  }

  initialized_ = true;
}

/**
 * @brief  Finaliser for PAPI  context.
 */

void meto::PAPIContext::finalize() {

  if(event_set_ != PAPI_NULL) {

    if( PAPI_cleanup_eventset(event_set_) != PAPI_OK ) {
      meto::error_handler(
                          "PAPIContext::finalize. Cleanup failed.",
                          EXIT_FAILURE);
    }
    if( PAPI_destroy_eventset(&event_set_)  != PAPI_OK ) {
      meto::error_handler(
                          "PAPIContext::finalize. Destroy failed.",
                          EXIT_FAILURE);
    }
  }

  initialized_ = false;
}
