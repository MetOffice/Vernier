
#include <chrono>

#include "prof_gettime.h"

time_point_t prof_gettime()
{
  return std::chrono::steady_clock::now();
}

