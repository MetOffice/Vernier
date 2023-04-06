/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   time_types.h
 * @brief  Contains custom type definitions for chrono durations and time 
 *         points.
 * 
 */

#include <chrono>

#ifndef TIME_TYPES_H
#define TIME_TYPES_H

// Type definitions for chrono steady clock time points and durations
using time_duration_t = std::chrono::duration<double>;
using time_point_t    = std::chrono::time_point<std::chrono::steady_clock, time_duration_t>;

#endif

