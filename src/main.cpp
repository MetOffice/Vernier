/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <unistd.h>

#include "profiler.h"

int main()
{

  // Start timing: noddy way, and using Profiler.

  double t1 = omp_get_wtime();
  auto prof_main = prof.start("MAIN");

  // Time a subroutine
  {
    auto prof_sub = prof.start("MAIN_SUB");

    //  #pragma omp parallel
    //  {
    //    size_t parallel_hash;
    //    for (int it=0; it<10000; ++it){
    //      prof.start("SUB_OMP", parallel_hash);
    //      prof.stop ("SUB_OMP", parallel_hash);
    //    }
    //  }

    sleep(10);

    prof.stop(prof_sub);
  }

  // Time nested subroutines on many threads.
#pragma omp parallel
  {
    auto prof_sub = prof.start("MAIN_SUB");
    sleep(10);

    // Time nested routine
    auto prof_sub2 = prof.start("MAIN_SUB2");
    sleep(10);

    prof.stop(prof_sub2);
    prof.stop(prof_sub);
  }

  // Give the main routine some substantial execution time.
  sleep(20);

  prof.stop(prof_main);
  prof.write();

  // Compute t1-t2 timing and output that.
  double t2 = omp_get_wtime();
  std::cout << "\n" << "Timing: " << t2 - t1 << "\n\n";

  return 0;
}

