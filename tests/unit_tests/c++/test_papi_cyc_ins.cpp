/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <cmath>
#include <cstdlib>

#include <gtest/gtest.h>
#include <papi.h>

#include "vernier.h"
#include "vernier_papi.h"

// ---------------------------------------------------------------------------
// Test: PAPI_TOT_CYC and PAPI_TOT_INS are both loaded and return positive
//       counts after executing a non-trivial region.
//
// Event indices (matching the order in VERNIER_PAPI_EVENTS1):
//   0 -> PAPI_TOT_CYC
//   1 -> PAPI_TOT_INS
// ---------------------------------------------------------------------------
TEST(PAPITest, TotCycTotInsTest) {

  // Request both events in order.
  setenv("VERNIER_PAPI_EVENTS1", "PAPI_TOT_CYC,PAPI_TOT_INS", /*overwrite=*/1);

  meto::vernier.init();

  // Skip gracefully if neither event could be loaded (e.g. no PMU access).
  if (meto::events_code.empty()) {
    meto::vernier.finalize();
    unsetenv("VERNIER_PAPI_EVENTS1");
    GTEST_SKIP() << "PAPI_TOT_CYC / PAPI_TOT_INS not available on this hardware.";
  }

  // Exactly two events must be active and in the expected order.
  ASSERT_EQ(meto::events_code.size(), 2u);
  EXPECT_EQ(meto::events_code[0].second, "PAPI_TOT_CYC");
  EXPECT_EQ(meto::events_code[1].second, "PAPI_TOT_INS");

  // Profile a region with meaningful work so both counters accumulate.
  auto prof = meto::vernier.start("CycInsRegion");

  volatile double acc = 0.0;
  for (int i = 1; i <= 1000000; ++i) {
    acc += std::sqrt(static_cast<double>(i));
  }
  (void)acc;

  meto::vernier.stop(prof);

  long long const tot_cyc = meto::vernier.get_total_metrics(prof, 0, 0);
  long long const tot_ins = meto::vernier.get_total_metrics(prof, 0, 1);

  // Both counters must be strictly positive.
  EXPECT_GT(tot_cyc, 0LL) << "Expected PAPI_TOT_CYC > 0.";
  EXPECT_GT(tot_ins, 0LL) << "Expected PAPI_TOT_INS > 0.";

  // Sanity: cycles >= instructions is not guaranteed (IPC can exceed 1),
  // but both must be at least as large as the loop iteration count since
  // each iteration requires at least one instruction and one cycle.
  EXPECT_GE(tot_cyc, 1'000'000LL)
      << "Expected at least 1 cycle per loop iteration.";
  EXPECT_GE(tot_ins, 1'000'000LL)
      << "Expected at least 1 instruction per loop iteration.";

  std::cout << "\n  PAPI_TOT_CYC : " << tot_cyc
            << "\n  PAPI_TOT_INS : " << tot_ins
            << "\n  IPC          : "
            << static_cast<double>(tot_ins) / static_cast<double>(tot_cyc)
            << "\n";

  meto::vernier.finalize();
  unsetenv("VERNIER_PAPI_EVENTS1");
}
