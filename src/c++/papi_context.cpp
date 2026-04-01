/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "vernier_papi.h"

#ifdef USE_PAPI
#include <papi.h>
#endif


#define PAPI_MAX_EVENTSET_PER_THREADS 1


/**
 * @brief  Constructor for a PAPI context.
 * @details This constructor does not initialise PAPI.
 * @note only one object at time. 
 */

meto::PAPIContext::PAPIContext() {
  count_++;
  assert(count==1);
  
  initialized_ = false;
}


/**
 * @brief  Initialise a Vernier PAPI context.
 */

void meto::PAPIContext::init() {


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
 * @brief  Finaliser for a Vernier PAPI context.
 */

void meto::PAPIContext::finalize() {
}

