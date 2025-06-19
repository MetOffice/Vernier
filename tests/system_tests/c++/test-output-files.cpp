/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2025 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// System test to check that the file outputs are correct

#include "vernier.h"
#include "vernier_mpi.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


/*
 * Check the first few lines of the output file
 */
bool check_output_file(std::string path, std::string format) {
  std::filebuf fb;
  std::string buffer;
  std::string expected;

  if(!fb.open(path, std::ios::in)) {
    std::cerr << "failed to open " << path << std::endl;
    return false;
  }

  std::istream input(&fb);

  // Check the header lines
  std::getline(input, buffer);
  if(buffer.compare("") != 0) {
    std::cerr << "Invalid line: " << buffer << std::endl;
    return false;
  }

  std::getline(input, buffer);
  if(buffer.compare(0, 8, "MPI task") != 0) {
    std::cerr << "Invalid line: " << buffer << std::endl;
    return false;
  }

  // Match the next line to the start of an output format
  if(format.compare("drhook") == 0) {
    expected = "Profiling on ";
  } else if(format.compare("threads") == 0) {
    // Skip extra blank line in threads format
    std::getline(input, buffer);
    expected = "region_name@thread_id";
  } else {
    std::cerr << "unknown format" << std::endl;
    return false;
  }

  std::getline(input, buffer);
  if(buffer.compare(0, expected.length(), expected) != 0) {
    std::cerr << "Invalid next line" << std::endl;
    std::cerr << "Got:      " << buffer << std::endl;
    std::cerr << "Expected: " << expected << std::endl;
    return false;
  }

  return true;
}


/*
 * Generate some profiler output and check the contents
 */
bool create_output(std::string mode, std::string format) {
  std::string path;
  meto::vernier.init(MPI_COMM_WORLD);

  // Create some data
  auto vnr_handle = meto::vernier.start("main");
  meto::vernier.stop(vnr_handle);

  setenv("VERNIER_OUTPUT_MODE", mode.c_str(), 1);
  setenv("VERNIER_OUTPUT_FORMAT", format.c_str(), 1);
  
  meto::vernier.write();

  meto::vernier.finalize();

  if(mode.compare("multi") == 0) {
    path = "vernier-output-0";
  }   else if(mode.compare("single") == 0) {
    path = "vernier-output-collated";
  } else {
    std::cerr << "unknown mode" << std::endl;
    return false;
  }
  
  if(!check_output_file(path, format)) {
    std::cerr << mode << " file " << format << " test failed" << std::endl;
    return false;
  }

  return true;
}


/*
 * Main function
 */
int main() {
  std::string modes[] = {"multi", "single"};
  std::string formats[] = {"drhook", "threads"};
  
  MPI_Init(NULL, NULL);

  for(auto mode : modes) {
    for(auto format : formats) {
      if(!create_output(mode, format)) {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
    }
  }

  MPI_Finalize();
}
