/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <gtest/gtest.h>
#include <papi.h>

#include "vernier.h"
#include "vernier_papi.h"

// ---------------------------------------------------------------------------
// Test: PAPI_TOT_INS is measured correctly across multiple threads and
//       multiple calls to the same sub-region.
//
// Layout:
//   MainRegion      (thread 0 only, called once)
//   └── WorkRegion  (called on every thread; thread T calls it T+1 times)
//
// Verification per thread T:
//   - call_count(WorkRegion, T) == T + 1
//   - tot_ins(WorkRegion, T)    >= (T+1) * MIN_INS_PER_CALL
//   - tot_ins(WorkRegion, T)    <= (T+1) * MAX_INS_PER_CALL
//
// Instruction count bounds for one WorkRegion call:
//   The body executes WORK_ITERS iterations.  Each iteration emits at minimum
//   ~7 scalar instructions: int-to-double conversion, a load and a store of
//   the volatile accumulator, sqrt, add, loop-counter increment, and branch.
//   A 2x upper-bound headroom covers preamble, postamble and any inlined
//   helper calls regardless of architecture.
// ---------------------------------------------------------------------------

static constexpr int  WORK_ITERS        = 100'000;
static constexpr long long MIN_INS_PER_CALL = 7LL  * WORK_ITERS;
static constexpr long long MAX_INS_PER_CALL = 20LL * WORK_ITERS;

// Perform a fixed, deterministic amount of work.
static void do_work() {
  volatile double acc = 0.0;
  for (int i = 1; i <= WORK_ITERS; ++i) {
    acc += std::sqrt(static_cast<double>(i));
  }
  (void)acc;
}

TEST(PAPITest, TotInsMultiThreadTest) {

  setenv("VERNIER_PAPI_EVENTS1", "PAPI_TOT_INS", /*overwrite=*/1);

  meto::vernier.init();

  // Skip gracefully if PAPI_TOT_INS is unavailable.
  if (meto::events_code.empty()) {
    meto::vernier.finalize();
    unsetenv("VERNIER_PAPI_EVENTS1");
    GTEST_SKIP() << "PAPI_TOT_INS not available on this hardware.";
  }

  ASSERT_EQ(meto::events_code.size(), 1u);
  EXPECT_EQ(meto::events_code[0].second, "PAPI_TOT_INS");

  // Shared storage: one WorkRegion hash per thread.
  int num_threads = 1;
  std::vector<size_t> work_hash_per_thread;

  auto prof_main = meto::vernier.start("MainRegion");

#pragma omp parallel default(none) shared(num_threads, work_hash_per_thread, meto::vernier)
  {
#pragma omp single
    {
#ifdef _OPENMP
      num_threads = omp_get_num_threads();
#endif
      work_hash_per_thread.resize(static_cast<size_t>(num_threads), 0);
    }

    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif

    // Thread T calls WorkRegion T+1 times so every thread has a distinct call
    // count, making it easier to verify per-thread accounting.
    size_t local_hash = 0;
    int const calls = tid + 1;
    for (int c = 0; c < calls; ++c) {
      local_hash = meto::vernier.start("WorkRegion");
      do_work();
      meto::vernier.stop(local_hash);
    }

#pragma omp critical
    { work_hash_per_thread[static_cast<size_t>(tid)] = local_hash; }
  }

  meto::vernier.stop(prof_main);

  // -------------------------------------------------------------------------
  // Verify per-thread call counts and instruction counts.
  // -------------------------------------------------------------------------
  std::cout << "\n  Thread | Calls | PAPI_TOT_INS\n"
            << "  -------|-------|-------------\n";

  for (int t = 0; t < num_threads; ++t) {
    size_t const hash  = work_hash_per_thread[static_cast<size_t>(t)];
    int    const calls = t + 1;

    // Call count
    auto const actual_calls = meto::vernier.get_call_count(hash, t);
    EXPECT_EQ(actual_calls, static_cast<unsigned long long>(calls))
        << "Thread " << t << ": unexpected call count.";

    // Instruction count: total across all calls on this thread.
    long long const tot_ins = meto::vernier.get_total_metrics(hash, t, 0);

    long long const lo = static_cast<long long>(calls) * MIN_INS_PER_CALL;
    long long const hi = static_cast<long long>(calls) * MAX_INS_PER_CALL;

    EXPECT_GE(tot_ins, lo)
        << "Thread " << t << ": PAPI_TOT_INS below lower bound "
        << lo << " for " << calls << " call(s).";
    EXPECT_LE(tot_ins, hi)
        << "Thread " << t << ": PAPI_TOT_INS above upper bound "
        << hi << " for " << calls << " call(s).";

    std::cout << "  " << t << "      | " << calls << "     | " << tot_ins
              << "\n";
  }

  meto::vernier.finalize();
  unsetenv("VERNIER_PAPI_EVENTS1");
}
