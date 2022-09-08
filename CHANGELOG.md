# Change log for the profiler package #

### Authors (in alphabetical order) ###

* Andrew Coughtrie, UK Met Office
* Maff Glover, UK Met Office
* Iva Kavcic, UK Met Office

### Commits to main ###

30/11/2021 PR #1: Add basic build system, unit-testing, and Doxygen documentation.\
17/12/2021 PR #16 for #3: Add option to fetch googletest and build in-situ.\
20/12/2021 PR #17: Create CMake GitHub Action to build and test. \
20/12/2021 PR #19 for #12: Use standard CMake project version variables for versioning. \
21/12/2021 PR #18: Add GitHub runner for building and deploying Doxygen documentation. \
21/12/2021 PR #22 for #13: Move install functionality in main CMakeLists.txt to cmake/Installation.cmake \
21/01/2022 PR #23: Fix version number passed to Doxygen. \
15/03/2022 PR #5 towards #2: Initial code import. \
08/07/2022 PR #27: Working Fortran (and C) interfaces. \
15/07/2022 PR #30 for #29: Unit testing for Fortran interface. \
04/08/2022 PR #32 for #31: Functionality improvements (walltime, swap to unordered_map, sort entries in output. \
10/08/2022 PR #41 for #37 Add null terminated strings in Fortran interface. \
16/08/2022 PR #48 Add hashtable.h to installed header files. \
30/08/2022 PR #51 for #34: Including the number of times a region is called. \
09/09/2022 PR #39 for #35: Initial profiler unit tests.
