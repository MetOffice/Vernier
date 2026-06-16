/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2025 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// System test to check that the file outputs are correct

#include "vernier.h"
#include "vernier_mpi.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// Define a simple assertion macro that prints the expected and actual values on
// failure.
#define M_Assert(expected, value, file_line)                                   \
  __M_Assert(expected, value, file_line, __FILE__, __LINE__)

void __M_Assert(const std::string &expected, const std::string &value,
                const int file_line, const char *file, const int line) {
  if (expected != value) {
    std::cerr << "Error: " << file << ":" << line << std::endl
              << "Assertion failed: source line: " << line
              << " against KGO file line: " << file_line << std::endl
              << "Expected: " << expected << std::endl
              << "Received: " << value << std::endl;

    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
}

/*
 * Check the first few lines of the output file
 */
bool check_output_file(const std::string &output_data_path,
                       const std::string &test_data_path) {
  std::filebuf output_file_buffer;
  std::filebuf test_file_buffer;
  std::string buffer;
  std::string expected;

  if (!test_file_buffer.open(test_data_path, std::ios::in)) {
    std::cerr << "failed to open " << test_data_path << std::endl;
    return false;
  }
  std::istream test_data(&test_file_buffer);

  if (!output_file_buffer.open(output_data_path, std::ios::in)) {
    std::cerr << "failed to open " << output_data_path << std::endl;
    return false;
  }
  std::istream input(&output_file_buffer);

  // Check the header lines
  for (int i = 0; i < 13; ++i) {
    std::getline(input, buffer);
    std::getline(test_data, expected);
    M_Assert(expected, buffer, i);
  }

  return true;
}

/*
 * Generate some profiler output and check the contents
 */
bool create_output(std::string mode, std::string format, int rank,
                   int total_ranks) {
  std::string path =
      format + "-" + std::to_string(total_ranks) + "core-vernier-output";
  meto::vernier.init(MPI_COMM_WORLD);
  std::string test_data_path = "test_data/" + format + "-" +
                               std::to_string(total_ranks) + "core-" + mode;
  // Create some data
  auto vnr_handle = meto::vernier.start("main");

  // Create some asymmetry in the number of calls made by different ranks.
  if (rank % 2 == 0) {
    auto vnr_handle_even_rank = meto::vernier.start("even_rank");
    meto::vernier.stop(vnr_handle_even_rank);
  }

  meto::vernier.stop(vnr_handle);

  setenv("VERNIER_OUTPUT_MODE", mode.c_str(), 1);
  setenv("VERNIER_OUTPUT_FORMAT", format.c_str(), 1);
  setenv("VERNIER_OUTPUT_FILENAME", path.c_str(), 1);

  meto::vernier.write();

  unsetenv("VERNIER_OUTPUT_FILENAME");
  unsetenv("VERNIER_OUTPUT_FORMAT");
  unsetenv("VERNIER_OUTPUT_MODE");

  meto::vernier.finalize();

  // Append the expected extension
  if (mode.compare("multi") == 0) {
    path.append("-0");
  } else if (mode.compare("single") == 0) {
    path.append("-collated");
  } else {
    std::cerr << "unknown mode" << std::endl;
    return false;
  }

  if (rank == 0 && !check_output_file(path, test_data_path)) {
    std::cerr << mode << " file " << format << " test failed" << std::endl;
    return false;
  }

  return true;
}

/*
 * Main function
 */
int main() {
  int rank;
  int total_ranks;
  std::string modes[] = {"multi", "single"};
  std::string formats[] = {"drhook", "default"};

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);

  for (auto mode : modes) {
    for (auto format : formats) {
      if (!create_output(mode, format, rank, total_ranks)) {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
    }
  }

  MPI_Finalize();
}
