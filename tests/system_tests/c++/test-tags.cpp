/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2025 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

// System test to check that output files with the correct filenames exist when
// the tag is allowed to default, and when a specific tag is given.

#include "vernier.h"
#include "vernier_mpi.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------

bool file_exists(std::string const &);
bool default_tag();
bool specify_tag();

//------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------

int main() {

  MPI_Init(NULL, NULL);

  // Quis custodiet ipsos custodes?
  if (file_exists("file-does-not-exist")) {
    std::cerr << "Non-existent file reported as existing!" << std::endl;
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Allow the tag to default.
  if (!default_tag()) {
    std::cerr << "Defaulting tag: output file with correct name does not exist."
              << std::endl;
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Specify a tag in vernier.init().
  if (!specify_tag()) {
    std::cerr << "Specific tag: output file with correct name does not exist."
              << std::endl;
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  MPI_Finalize();
}

//------------------------------------------------------------------------------
// Default tag
//------------------------------------------------------------------------------

bool default_tag() {

  meto::vernier.init(MPI_COMM_WORLD);

  auto vnr_handle = meto::vernier.start("main");
  meto::vernier.stop(vnr_handle);

  meto::vernier.write();
  meto::vernier.finalize();

  // Check that a file with correct name exists.
  return file_exists("vernier-output-0");
}

//------------------------------------------------------------------------------
// Specify tag
//------------------------------------------------------------------------------

bool specify_tag() {

  meto::vernier.init(MPI_COMM_WORLD, "TEST-TAG");

  auto vnr_handle = meto::vernier.start("main");
  meto::vernier.stop(vnr_handle);

  meto::vernier.write();
  meto::vernier.finalize();

  // Check that a file with correct name exists.
  return file_exists("vernier-output-TEST-TAG-0");
}

//------------------------------------------------------------------------------
// Check whether a file exists.
//------------------------------------------------------------------------------

bool file_exists(std::string const &fname) {
  std::ifstream file(fname);
  return file.good();
}