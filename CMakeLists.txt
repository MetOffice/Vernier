# Set the minimum version of CMake required by the build system.
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Include file containing in-source build prevention.
include(cmake/PreventInSourceBuilds.cmake)

# Set project name and the languages required to build the project and a
# project description.
project(profiler LANGUAGES CXX
        DESCRIPTION "Met Office profiler")

# Set project versioning.
# TODO - Issue #12 Move versioning to the project settings above.
set(SOVERSION 0)
set(VERSION 0.1.0)

# Include general project setting files.
include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Doxygen.cmake)
enable_doxygen()

# OpenMP required
find_package(OpenMP REQUIRED) 

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

# All below is for installation and creation of *.cmake files for use with the
# find_package cmake function.
# TODO - Issue #13 Move installation functionality to file in cmake directory.
install(
    EXPORT "${CMAKE_PROJECT_NAME}Targets"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
    NAMESPACE   ${CMAKE_PROJECT_NAME}::
    FILE       "${CMAKE_PROJECT_NAME}Targets.cmake"
    )
    
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_PROJECT_NAME}Config.cmake.in"
    "${CMAKE_PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    PATH_VARS CMAKE_INSTALL_LIBDIR 
    )
    
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${VERSION}
        COMPATIBILITY SameMajorVersion
)
    
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    )
