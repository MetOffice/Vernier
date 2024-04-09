/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2022 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "vernier_get_wtime.h"
#include "error_handler.h"
#include <time.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#define BILLION  1000000000L

/**
 * @brief Returns the current time.
 *
 * @returns A timespec struct which has the timings in seconds and nanoseconds.
 */

double meto::vernier_get_wtime()
{
    struct timespec point;
    double time;
    if (clock_gettime( CLOCK_REALTIME, &point) == -1)
    {
        //std::runtime_error("clock gettime");
        //exit(EXIT_FAILURE);
        error_handler("clock gettime", EXIT_FAILURE);
    }
    time = static_cast<double>(point.tv_sec) + static_cast<double>(point.tv_nsec)/BILLION;
    return time;
}
