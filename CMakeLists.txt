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
project(vernier LANGUAGES CXX C Fortran
        DESCRIPTION "Met Office Vernier profiler"
        VERSION 0.4.0
        HOMEPAGE_URL "https://github.com/MetOffice/Vernier")

# OpenMP required
find_package(OpenMP 2.5 REQUIRED)

# MPI required?
option(ENABLE_MPI "Enable MPI" ON)

if (ENABLE_MPI)
  find_package(MPI REQUIRED)
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})
else()
  message(STATUS "ENABLE_MPI disabled.")
endif()

# Defines some standard install directories such as $(prefix)/include.
include(GNUInstallDirs)

# Instructions only relevant if current project is Vernier
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

        # Include general project setting files.
        include(cmake/StandardProjectSettings.cmake)
        include(cmake/CompilerWarnings.cmake)

        # Include the testing setup.
        include(cmake/TestingSetup.cmake)

        # Include documentation related files.
        include(cmake/Doxygen.cmake)
        include(cmake/Sphinx.cmake)

        # Include option to build with/without OpenMP
        include(cmake/BuildOptions.cmake)

endif()

# Add src directory to build.
add_subdirectory(src)

# Include the installation options.
include(cmake/Installation.cmake)

# Allow the maximum label sizes to be tuned
if(STRING_LENGTH)
  if(STRING_LENGTH GREATER_EQUAL 100)
    message(STATUS "Max caliper label length is ${STRING_LENGTH}")
    target_compile_definitions(${CMAKE_PROJECT_NAME} INTERFACE
      PROF_STRING_BUFFER_LENGTH=${STRING_LENGTH})
  else()
    message(WARNING "Max caliper label length is too small - using 100")
  endif()
endif()

include(cmake/PkgConfig.cmake)
