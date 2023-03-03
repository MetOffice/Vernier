# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# Set the minimum version of CMake required by the build system.
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Include file containing in-source build prevention.
include(cmake/PreventInSourceBuilds.cmake)

# Set project name and the languages required to build the project and a
# project description. Also set the version of the library being built.
project(profiler LANGUAGES CXX C Fortran
        DESCRIPTION "Met Office profiler"
        VERSION 0.1.0)

# Include general project setting files.
include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)

# Defines some standard install directories such as $(prefix)/include.
include(GNUInstallDirs)

# OpenMP required
find_package(OpenMP 2.5 REQUIRED)

# Add src directory to build.
add_subdirectory(src)

# Include the testing setup.
include(cmake/TestingSetup.cmake)

# Include the installation options.
include(cmake/Installation.cmake)

# Include documentation related files.
include(cmake/Doxygen.cmake)
include(cmake/Sphinx.cmake)
add_dependencies(sphinx doxygen)
