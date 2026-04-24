/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// Some of the content of this file has been produced with the assistance of
// Met Office Github Copilot Enterprise

// System test: verify PAPI_FP_OPS counts are consistent across three
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
// In Run 1 and Run 3, every thread profiles exactly one do_work()
// call, so PAPI_FP_OPS should be very similar across threads and
// across the two runs.  In Run 2 the region wraps the entire parallel
// block; only thread 0 holds the Run2Region entry, and its count
// should match the sum of per-thread values of the other runs.
//
// A single init / write / finalize cycle is used for all three runs.
// The test writes threads-format output.

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "vernier.h"
#include "vernier_mpi.h"

#ifdef USE_PAPI
#include "vernier_papi.h"
#include <papi.h>
#endif

static constexpr int WORK_ITERS = 100'000;
static constexpr long long MIN_OPS_PER_CALL = 1LL * WORK_ITERS;
static constexpr long long MAX_OPS_PER_CALL = 3LL * WORK_ITERS;

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
  std::string buf;

  std::getline(in, buf);
  if (buf != "") {
    std::cerr << "check_threads_output: expected blank line, got: '" << buf
              << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf.compare(0, 9, "Task 1 of") != 0) {
    std::cerr << "check_threads_output: expected 'Task 1 of ...', got: '" << buf
              << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf != "") {
    std::cerr << "check_threads_output: expected blank line, got: '" << buf
              << "'\n";
    return false;
  }

  std::getline(in, buf);
  if (buf.compare(0, 21, "region_name@thread_id") != 0) {
    std::cerr << "check_threads_output: expected column header, got: '" << buf
              << "'\n";
    return false;
  }

  return true;
}

// ---------------------------------------------------------------------------
int main() {
  MPI_Init(NULL, NULL);

  setenv("VERNIER_PAPI_EVENTS1", "PAPI_FP_OPS", /*overwrite=*/1);

  meto::vernier.init();

#ifdef USE_PAPI
  // Skip gracefully if PAPI_FP_OPS is unavailable on this hardware.
  if (meto::events_code.empty()) {
    meto::vernier.finalize();
    unsetenv("VERNIER_PAPI_EVENTS1");
    std::cout << "PAPI_FP_OPS not available on this hardware – test skipped.\n";
    MPI_Finalize();
    return EXIT_SUCCESS;
  }
#endif

  // -------------------------------------------------------------------------
  // Run 1: vernier region inside the parallel region.
  // -------------------------------------------------------------------------
  int num_threads = 1;
  std::vector<size_t> run1_hashes;

#pragma omp parallel default(none)                                             \
    shared(num_threads, run1_hashes, meto::vernier)
  {
#pragma omp single
    {
#ifdef _OPENMP
      num_threads = omp_get_num_threads();
#endif
      run1_hashes.resize(static_cast<size_t>(num_threads), 0);
    }

    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif

    size_t const h = meto::vernier.start("Run1Region");
    do_work();
    meto::vernier.stop(h);
#pragma omp critical
    { run1_hashes[static_cast<size_t>(tid)] = h; }
  }

  // -------------------------------------------------------------------------
  // Run 2: vernier region wraps the parallel region.
  // -------------------------------------------------------------------------
  size_t const run2_hash = meto::vernier.start("Run2Region");

#pragma omp parallel default(none)
  { do_work(); }

  meto::vernier.stop(run2_hash);

  // -------------------------------------------------------------------------
  // Run 3: per-thread outer region, nested inner region via extra_routine.
  // -------------------------------------------------------------------------
  std::vector<size_t> run3_outer_hashes(static_cast<size_t>(num_threads), 0);
  std::vector<size_t> run3_inner_hashes(static_cast<size_t>(num_threads), 0);

#pragma omp parallel default(none)                                             \
    shared(run3_outer_hashes, run3_inner_hashes, meto::vernier)
  {
    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif

    size_t const outer_h = meto::vernier.start("OuterRegion");
    size_t const inner_h = extra_routine();
    meto::vernier.stop(outer_h);

    run3_outer_hashes[static_cast<size_t>(tid)] = outer_h;
    run3_inner_hashes[static_cast<size_t>(tid)] = inner_h;
  }

  // -------------------------------------------------------------------------
  // Collect per-thread metrics before write / finalize.
  // Run2Region is started/stopped on thread 0 only (outside the parallel block)
  // so only thread-0 metrics exist for that region.
  // -------------------------------------------------------------------------
#ifdef USE_PAPI
  std::vector<long long> run1_ins(static_cast<size_t>(num_threads));
  std::vector<long long> run3_ins(static_cast<size_t>(num_threads));
  for (int t = 0; t < num_threads; ++t) {
    run1_ins[static_cast<size_t>(t)] = meto::vernier.get_total_metrics(
        run1_hashes[static_cast<size_t>(t)], t, 0);
    run3_ins[static_cast<size_t>(t)] = meto::vernier.get_total_metrics(
        run3_inner_hashes[static_cast<size_t>(t)], t, 0);
  }
  long long const run2_ins = meto::vernier.get_total_metrics(run2_hash, 0, 0);
#endif

  // -------------------------------------------------------------------------
  // Write threads-format output, then finalize.
  // -------------------------------------------------------------------------
  static constexpr char OUTPUT_FILENAME[] = "papi-omp-threads-output";

  setenv("VERNIER_OUTPUT_FORMAT", "threads", /*overwrite=*/1);
  setenv("VERNIER_OUTPUT_MODE", "multi", /*overwrite=*/1);
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
  std::cout << "\n  Run 1 (Run1Region) and Run 3 (InnerRegion) — all threads:\n"
            << "  Thread | Run1Region       | InnerRegion\n"
            << "  -------|------------------|------------------\n";
  for (int t = 0; t < num_threads; ++t) {
    std::cout << "  " << t << "      | " << run1_ins[static_cast<size_t>(t)]
              << "  | " << run3_ins[static_cast<size_t>(t)] << "\n";
  }
  std::cout
      << "\n  Run 2 (Run2Region) — thread 0 only (region wraps parallel):\n"
      << "  Thread | Run2Region\n"
      << "  -------|------------------\n"
      << "  0      | " << run2_ins << "\n";

  // Bounds check: Run1Region and InnerRegion on all threads.
  for (int t = 0; t < num_threads; ++t) {
    if (run1_ins[static_cast<size_t>(t)] < MIN_OPS_PER_CALL ||
        run1_ins[static_cast<size_t>(t)] > MAX_OPS_PER_CALL) {
      std::cerr << "Run1Region thread " << t << ": PAPI_FP_OPS "
                << run1_ins[static_cast<size_t>(t)] << " out of bounds ["
                << MIN_OPS_PER_CALL << ", " << MAX_OPS_PER_CALL << "]\n";
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (run3_ins[static_cast<size_t>(t)] < MIN_OPS_PER_CALL ||
        run3_ins[static_cast<size_t>(t)] > MAX_OPS_PER_CALL) {
      std::cerr << "InnerRegion thread " << t << ": PAPI_FP_OPS "
                << run3_ins[static_cast<size_t>(t)] << " out of bounds ["
                << MIN_OPS_PER_CALL << ", " << MAX_OPS_PER_CALL << "]\n";
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }
  // Bounds check: Run2Region thread 0.
  if (run2_ins < num_threads * MIN_OPS_PER_CALL ||
      run2_ins > num_threads * MAX_OPS_PER_CALL) {
    std::cerr << "Run2Region thread 0: PAPI_FP_OPS " << run2_ins
              << " out of bounds [" << num_threads * MIN_OPS_PER_CALL << ", "
              << num_threads * MAX_OPS_PER_CALL << "]\n";
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
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
