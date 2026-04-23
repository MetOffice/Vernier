/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// System test: verify PAPI_TOT_INS counts are consistent across three
// OpenMP/profiling patterns.
//
// Run 1 – vernier region inside the parallel region (each thread profiles its
//          own work):
//
//            #pragma omp parallel {
//              start("Run1Region");
//              do_work();
//              stop("Run1Region");
//            }
//
// Run 2 – vernier region wraps the parallel region:
//
//            start("Run2Region");
//            #pragma omp parallel { do_work(); }
//            stop("Run2Region");
//
// Run 3 – per-thread outer region with nested inner region via extra_routine:
//
//            #pragma omp parallel {
//              start("OuterRegion");
//              extra_routine();   // start("InnerRegion"); do_work(); stop();
//              stop("OuterRegion");
//            }
//
// On thread 0, each pattern profiles exactly one do_work() call.  The
// PAPI_TOT_INS values for Run1Region (Run 1), Run2Region (Run 2), and
// InnerRegion (Run 3) should therefore be very similar.
//
// A single init / write / finalize cycle is used for all three runs.
// The test writes threads-format output.

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "vernier.h"
#include "vernier_mpi.h"

#ifdef USE_PAPI
#include <papi.h>
#include "vernier_papi.h"
#endif

static constexpr int       WORK_ITERS       = 100'000;
static constexpr long long MIN_INS_PER_CALL = 7LL  * WORK_ITERS;
static constexpr long long MAX_INS_PER_CALL = 20LL * WORK_ITERS;

// ---------------------------------------------------------------------------
// Perform a fixed, deterministic amount of work.
// ---------------------------------------------------------------------------
static void do_work() {
  volatile double acc = 0.0;
  for (int i = 1; i <= WORK_ITERS; ++i) {
    acc += std::sqrt(static_cast<double>(i));
  }
  (void)acc;
}

// ---------------------------------------------------------------------------
// Wrap do_work in its own profiled region; return the hash for later metric
// retrieval.
// ---------------------------------------------------------------------------
static size_t extra_routine() {
  size_t const h = meto::vernier.start("InnerRegion");
  do_work();
  meto::vernier.stop(h);
  return h;
}

// ---------------------------------------------------------------------------
// Check the threads-format header produced by Vernier.
// Expected structure:
//   (blank line)
//   Task 1 of N
//   (blank line)
//   region_name@thread_id  ...
// ---------------------------------------------------------------------------
static bool check_threads_output(std::string const &path) {
  std::filebuf fb;
  if (!fb.open(path, std::ios::in)) {
    std::cerr << "check_threads_output: failed to open " << path << "\n";
    return false;
  }

  std::istream in(&fb);
  std::string  buf;

  std::getline(in, buf);
  if (buf != "") {
    std::cerr << "check_threads_output: expected blank line, got: '"
              << buf << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf.compare(0, 9, "Task 1 of") != 0) {
    std::cerr << "check_threads_output: expected 'Task 1 of ...', got: '"
              << buf << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf != "") {
    std::cerr << "check_threads_output: expected blank line, got: '"
              << buf << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf.compare(0, 21, "region_name@thread_id") != 0) {
    std::cerr << "check_threads_output: expected column header, got: '"
              << buf << "'\n";
    return false;
  }

  return true;
}

// ---------------------------------------------------------------------------
int main() {
  MPI_Init(NULL, NULL);

  setenv("VERNIER_PAPI_EVENTS1", "PAPI_TOT_INS", /*overwrite=*/1);

  meto::vernier.init();

#ifdef USE_PAPI
  // Skip gracefully if PAPI_TOT_INS is unavailable on this hardware.
  if (meto::events_code.empty()) {
    meto::vernier.finalize();
    unsetenv("VERNIER_PAPI_EVENTS1");
    std::cout << "PAPI_TOT_INS not available on this hardware – test skipped.\n";
    MPI_Finalize();
    return EXIT_SUCCESS;
  }
#endif

  // -------------------------------------------------------------------------
  // Run 1: vernier region inside the parallel region.
  // -------------------------------------------------------------------------
  size_t run1_hash = 0;

#pragma omp parallel default(none) shared(run1_hash, meto::vernier)
  {
    size_t const h = meto::vernier.start("Run1Region");
    do_work();
    meto::vernier.stop(h);
#pragma omp single
    { run1_hash = h; }
  }

  // -------------------------------------------------------------------------
  // Run 2: vernier region wraps the parallel region.
  // -------------------------------------------------------------------------
  size_t const run2_hash = meto::vernier.start("Run2Region");

#pragma omp parallel
  { do_work(); }

  meto::vernier.stop(run2_hash);

  // -------------------------------------------------------------------------
  // Run 3: per-thread outer region, nested inner region via extra_routine.
  // -------------------------------------------------------------------------
  size_t run3_inner_hash = 0;

#pragma omp parallel default(none) shared(run3_inner_hash, meto::vernier)
  {
    size_t const outer_h = meto::vernier.start("OuterRegion");
    size_t const inner_h = extra_routine();
    meto::vernier.stop(outer_h);
#pragma omp single
    { run3_inner_hash = inner_h; }
  }

  // -------------------------------------------------------------------------
  // Collect thread-0 metrics before write / finalize.
  // -------------------------------------------------------------------------
#ifdef USE_PAPI
  long long const ins[3] = {
    meto::vernier.get_total_metrics(run1_hash,       0, 0),
    meto::vernier.get_total_metrics(run2_hash,       0, 0),
    meto::vernier.get_total_metrics(run3_inner_hash, 0, 0),
  };
#endif

  // -------------------------------------------------------------------------
  // Write threads-format output, then finalize.
  // -------------------------------------------------------------------------
  static constexpr char OUTPUT_FILENAME[] = "papi-omp-threads-output";

  setenv("VERNIER_OUTPUT_FORMAT",   "threads",       /*overwrite=*/1);
  setenv("VERNIER_OUTPUT_MODE",     "multi",         /*overwrite=*/1);
  setenv("VERNIER_OUTPUT_FILENAME", OUTPUT_FILENAME, /*overwrite=*/1);

  meto::vernier.write();
  meto::vernier.finalize();

  unsetenv("VERNIER_OUTPUT_FILENAME");
  unsetenv("VERNIER_OUTPUT_MODE");
  unsetenv("VERNIER_OUTPUT_FORMAT");
  unsetenv("VERNIER_PAPI_EVENTS1");

  // -------------------------------------------------------------------------
  // Print table and verify bounds.
  // -------------------------------------------------------------------------
#ifdef USE_PAPI
  std::cout << "\n  Run | Region       | PAPI_TOT_INS (thread 0)\n"
            << "  ----|--------------|------------------------\n"
            << "  1   | Run1Region   | " << ins[0] << "\n"
            << "  2   | Run2Region   | " << ins[1] << "\n"
            << "  3   | InnerRegion  | " << ins[2] << "\n";

  static const char *const region_names[3] = {
    "Run1Region", "Run2Region", "InnerRegion"
  };

  for (int r = 0; r < 3; ++r) {
    if (ins[r] < MIN_INS_PER_CALL) {
      std::cerr << region_names[r] << ": PAPI_TOT_INS " << ins[r]
                << " below lower bound " << MIN_INS_PER_CALL << "\n";
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (ins[r] > MAX_INS_PER_CALL) {
      std::cerr << region_names[r] << ": PAPI_TOT_INS " << ins[r]
                << " above upper bound " << MAX_INS_PER_CALL << "\n";
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }
#endif

  // -------------------------------------------------------------------------
  // Check the threads-format output file header.
  // -------------------------------------------------------------------------
  std::string const output_path = std::string(OUTPUT_FILENAME) + "-0";
  if (!check_threads_output(output_path)) {
    std::cerr << "Threads output file check failed: " << output_path << "\n";
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
