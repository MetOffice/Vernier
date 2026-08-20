/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <chrono>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include "error_handler.h"
#include "hashvec_handler.h"
#include "vernier.h"
#include "vernier_mpi.h"

using ::testing::ExitedWithCode;

// Forward declarations
void check_file_exists_and_remove(std::string const);

//
//  Tests and death tests related to Vernier class members.
//

// Make sure the code exits when a hash mismatch happens.
TEST(DeathTest, WrongHashTest) {

  meto::vernier.init();

  EXPECT_EXIT(
      {
        // Start main
        const auto &prof_main = meto::vernier.start("Chocolate");

        // A subregion
        const auto &prof_sub = meto::vernier.start("Vanilla");
        meto::vernier.stop(prof_sub);

        // Wrong hash in Vernier.stop()
        meto::vernier.stop(prof_sub);

        // Eventually stop prof_main to avoid Wunused telling me off...
        meto::vernier.stop(prof_main);
      },
      ExitedWithCode(EXIT_FAILURE), "EMERGENCY STOP: hashes don't match.");

  meto::vernier.finalize();
}

// Tests for a segfault when stopping before anything else.
TEST(DeathTest, StopBeforeStartTest) {

  EXPECT_EXIT(
      {
        const auto prof_main = std::hash<std::string_view>{}("Main");

        // Stop Vernier before anything is done
        meto::vernier.stop(prof_main);
      },
      ExitedWithCode(EXIT_FAILURE),
      "EMERGENCY STOP: stop called before start calliper.");
}

// Vernier is not initialised before first start() call.
TEST(DeathTest, StartBeforeInit) {
  // clang-format off
  EXPECT_EXIT({ meto::vernier.start("MAIN"); }, ExitedWithCode(EXIT_FAILURE),
              "Vernier::start_part1. Vernier not initialised.");
  // clang-format on
}

// MPI is initialised, but the passed communicator handle is
// MPI_COMM_NULL.
TEST(DeathTest, NullCommunicatorPassed) {
  [[maybe_unused]] int ierr;

  EXPECT_EXIT(
      { meto::vernier.init(MPI_COMM_NULL); }, ExitedWithCode(EXIT_FAILURE),
      "MPIContext::init. MPI initialized, but null communicator passed.");

  meto::vernier.finalize();
}

// Check that uninitialised MPI is caught in the write functionality.
TEST(DeathTest, VernierUninitialisedInWrite) {

  // No init() called yet, so MPI context not initialised.
  // clang-format off
  EXPECT_EXIT({ meto::vernier.write(); }, ExitedWithCode(EXIT_FAILURE),
              "Vernier::write. Vernier not initialised.");
  // clang-format on
}

// Check that uninitialised MPI is caught in the affinity write functionality.
TEST(DeathTest, VernierUninitialisedInAffinityWrite) {

  // No init() called yet, so MPI context not initialised.
  // clang-format off
  EXPECT_EXIT({ meto::vernier.write_affinity(); }, ExitedWithCode(EXIT_FAILURE),
              "Vernier::write_affinity. Vernier not initialised.");
  // clang-format on
}

// The traceback array is not a growable vector. Check that the code exits
// when available array elements are exhausted.
TEST(DeathTest, TooManyTracebackEntries) {

  meto::vernier.init();

  EXPECT_EXIT(
      {
        const int beyond_maximum = PROF_MAX_TRACEBACK_SIZE + 1;
        for (int i = 0; i < beyond_maximum; ++i) {
          [[maybe_unused]] auto prof_handle =
              meto::vernier.start("TracebackEntry");
        }
      },
      ExitedWithCode(EXIT_FAILURE),
      "EMERGENCY STOP: Traceback array exhausted.");

  meto::vernier.finalize();
}

// Tests the correct io mode is set. If not set correctly it will exit.
TEST(DeathTest, InvalidIOModeTest) {
  EXPECT_EXIT(
      {
        meto::MPIContext mpi_context;

        const char *invalidIOMode = "invalid-mode";
        setenv("VERNIER_OUTPUT_MODE", invalidIOMode, 1);

        meto::HashVecHandler object(mpi_context);
      },
      ExitedWithCode(EXIT_FAILURE), "Invalid IO mode choice");
}

// Check that the Vernier affinity functionality produces files with expected
// filenames.
TEST(VernierTest, WriteAffinity) {

  meto::vernier.init(MPI_COMM_WORLD);

  std::string const seedname = "vernier-affinity";
  std::string const extension = ".txt";
  std::string fname;

  // Test with default filename
  meto::vernier.write_affinity();
  fname = seedname + extension;
  check_file_exists_and_remove(fname);

  // Test with tag
  meto::vernier.write_affinity("tag");
  fname = seedname + "-tag" + extension;
  check_file_exists_and_remove(fname);

  // Test with comm, no tag
  meto::vernier.write_affinity_with_comm(MPI_COMM_WORLD);
  fname = seedname + extension;
  check_file_exists_and_remove(fname);

  // Test with comm and tag
  meto::vernier.write_affinity_with_comm(MPI_COMM_WORLD, "commtag");
  fname = seedname + "-commtag" + ".txt";
  check_file_exists_and_remove(fname);

  meto::vernier.finalize();
}

// Helper routine to check whether a file exists, then remove it. Happens on MPI
// rank 0 only.
void check_file_exists_and_remove(std::string const fname) {

  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == 0) {
    EXPECT_TRUE(std::filesystem::exists(fname));
    EXPECT_TRUE(std::filesystem::remove(fname));
  }
}
