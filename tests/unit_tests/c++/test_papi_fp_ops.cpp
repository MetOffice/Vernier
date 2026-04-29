/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// Some of the content of this file has been produced with the assistance of
// Met Office Github Copilot Enterprise

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <gtest/gtest.h>
#include <papi.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "vernier.h"
#include "vernier_papi.h"

// ---------------------------------------------------------------------------
// Test: only PAPI_FP_OPS is loaded and its counter is positive after
//       performing floating-point work inside a Vernier region.
// ---------------------------------------------------------------------------
TEST(PAPITest, FPOpsOnlyTest) {

  // Ensure only PAPI_FP_OPS is requested.
  setenv("VERNIER_PAPI_EVENTS", "PAPI_FP_OPS", /*overwrite=*/1);

  // Probe before vernier.init(): if the event is unavailable, vernier.init()
  // would call error_handler → std::exit / MPI_Abort.  Skipping here prevents
  // that hard termination from killing the test binary.
  if (!meto::papi_events_probe()) {
    unsetenv("VERNIER_PAPI_EVENTS");
    GTEST_SKIP() << "PAPI_FP_OPS not available on this hardware.";
  }

  meto::vernier.init();

  // Fallback: skip if no events were actually loaded (env var unset path).
  if (meto::events_code.empty()) {
    meto::vernier.finalize();
    unsetenv("VERNIER_PAPI_EVENTS");
    GTEST_SKIP() << "PAPI_FP_OPS not available on this hardware.";
  }

  // Exactly one event must be active.
  ASSERT_EQ(meto::events_code.size(), 1u);
  EXPECT_EQ(meto::events_code[0].second, "PAPI_FP_OPS");

  // Profile a region that performs a known amount of FP work.
  auto prof = meto::vernier.start("FPRegion");

  volatile double acc = 0.0;
#pragma omp parallel for reduction(+ : acc)
  for (int i = 1; i <= 1000000; ++i) {
    acc += std::sqrt(static_cast<double>(i));
  }
  // Prevent the compiler from optimising the loop away.
  (void)acc;

  meto::vernier.stop(prof);

  long long const fp_ops = meto::vernier.get_total_metrics(prof, 0, 0);

  // Lower bound: at least 1,000,000 FP ops.
  // Each iteration executes at minimum one FADD/VADDSD (the += accumulation).
  // std::sqrt cannot be fused into an FMA, so it contributes a separate
  // FSQRT/VSQRTSD instruction.  volatile on acc prevents FMA fusion of the
  // addition as well.  Even on PMUs that do not count SQRT (e.g. some Intel
  // configurations of PAPI_FP_OPS), the additions alone reach 1,000,000.
  EXPECT_GE(fp_ops, 1'000'000LL)
      << "Expected at least 1 FP op per iteration (accumulation).";

  // Upper bound: no more than 3,000,000 FP ops.
  // The theoretical maximum is 2 ops/iteration (FSQRT + FADD = 2,000,000).
  // A factor of 3x headroom accommodates PMUs that count internal micro-ops
  // or repeat-count for iterative SQRT implementations on some architectures.
  EXPECT_LE(fp_ops, 3'000'000LL)
      << "Expected at most 3 FP ops per iteration (sanity upper bound).";

  std::cout << "\n  PAPI_FP_OPS : " << fp_ops << "\n";

#ifdef _OPENMP
  std::cout << "\n  THREADS     : " << omp_get_max_threads() << "\n";
#endif

  meto::vernier.finalize();
  unsetenv("VERNIER_PAPI_EVENTS");
}
