
#include <time.h>

#define BILLION  1'000'000'000L

double prof_gettime()
{
  struct timespec monotime;
  clock_gettime(CLOCK_MONOTONIC, &monotime);

  double time = static_cast<double>(monotime.tv_sec) 
              + static_cast<double>(monotime.tv_nsec) / static_cast<double>(BILLION);

  return time;
}

