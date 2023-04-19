/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "prof_gettime.h"

/**
 * @brief Returns the current time.
 *
 * @returns The present time point.
 */

time_point_t prof_gettime()
{
  return std::chrono::steady_clock::now();
}

