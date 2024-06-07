/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   vernier_gettime().h
 * @brief  Declares time-measurement functionality.
 *
 * Contains abstractions for time points, time durations and the clock itself.
 */

#ifndef VERNIER_GETTIME_H
#define VERNIER_GETTIME_H

#include <chrono>

namespace meto {

// Type definitions for chrono steady clock time points and durations
using time_duration_t = std::chrono::duration<double>;
using time_point_t =
    std::chrono::time_point<std::chrono::steady_clock, time_duration_t>;

// Function prototypes
time_point_t vernier_gettime();

} // namespace meto
#endif
