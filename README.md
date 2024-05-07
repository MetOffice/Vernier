<!--- 
(c) Crown copyright 2023 Met Office. All rights reserved.
The file LICENCE, distributed with this code, contains details of the terms
under which the code may be used.
 --->

# Vernier

[![Build](https://github.com/MetOffice/Vernier/actions/workflows/build.yml/badge.svg)](https://github.com/MetOffice/Vernier/actions/workflows/build.yml)
[![Docs](https://github.com/MetOffice/Vernier/actions/workflows/documentation.yml/badge.svg)](https://github.com/MetOffice/Vernier/actions/workflows/documentation.yml)

Profiler for scientific code on HPC platforms.

Documentation for the project can be found [here](https://metoffice.github.io/Vernier).

### Requirements

- C++17 compatible compiler.
- CMake version 3.13 or newer.
- GoogleTest version 1.11 or newer.

### Supported Compilers

The code has been tested with the following compilers:
- GCC versions: 10.2.0, 11.2.0
- Clang versions: 12.0 

### Libraries 

- OpenMP 4.5 
- MPICH 3.3.2

### Testing Framework and Documentation

The testing framework is GoogleTest (1.11.0).  
Source code documentation is generated using Doxygen (minimum 1.8.5).

### Building profiler

CMake build options are available [here](https://metoffice.github.io/Vernier/#Options).

**To build:**

In project root directory:
~~~~~~~~~~~~~~~~shell
mkdir build
cd build
cmake ..
make
make test
~~~~~~~~~~~~~~~~

#### Documentation

The HTML Doxygen documentation is built by default when building the project and 
the generated files can be found in `<build>/html`.

To build only the Doxygen docs use `make doxygen-docs`. On installation the
HTML pages are copied to `<installation_root>/share/docs`.

#### Unit Test Coverage

The unit test coverage is generated using `gcovr` which wraps `gcov` with additional
functionality, such as multiple output format options.

A detailed coverage report can be found [here](https://metoffice.github.io/profiler/coverage/).
