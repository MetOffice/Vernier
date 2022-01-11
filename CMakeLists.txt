# Set the minimum version of CMake required by the build system.
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Include file containing in-source build prevention.
include(cmake/PreventInSourceBuilds.cmake)

# Set project name and the languages required to build the project and a
# project description. Also set the version of the library being built.
project(profiler LANGUAGES CXX
        DESCRIPTION "Met Office profiler"
        VERSION 0.1.0)

# Include general project setting files.
include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Doxygen.cmake)
enable_doxygen()

# OpenMP required
find_package(OpenMP 2.5 REQUIRED) 

# Trick to make it easier to add warnings and options to libs/executables.
add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

# Defines some standard install directories such as $(prefix)/include.
include(GNUInstallDirs)

# Add src directory to build.
add_subdirectory(src)

# Include the testing setup.
include(cmake/TestingSetup.cmake)

# Include the installation options.
include(cmake/Installation.cmake)
