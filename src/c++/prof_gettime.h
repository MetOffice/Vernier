
#ifndef PROF_GETTIME_H
#define PROF_GETTIME_H

// Type definitions for chrono steady clock time points and durations
using time_duration_t = std::chrono::duration<double>;
using time_point_t    = std::chrono::time_point<std::chrono::steady_clock, time_duration_t>;

// Function prototypes
time_point_t prof_gettime();

#endif

