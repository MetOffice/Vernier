/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   clock.h
 * @brief  Contains a method for retrieving the current time point and stores
 *         the overall program duration.
 * 
 */

#include "time_types.h"

#ifndef PROFILER_CLOCK_H
#define PROFILER_CLOCK_H

/**
 * @brief  Nests the clock get_time method and program duration variable
 *         together.
 * 
 */

struct Clock {

    // Function prototypes
    time_point_t get_time();

    // Program duration
    time_duration_t program_duration;

};

inline Clock prof_clock;

#endif