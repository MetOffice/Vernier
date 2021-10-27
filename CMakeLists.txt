cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
include(cmake/PreventInSourceBuilds.cmake)

project(profiler LANGUAGES CXX
        DESCRIPTION "Met Office Profiler")

set(SOVERSION 0)
set(VERSION 0.1.0)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Doxygen.cmake)
enable_doxygen()

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

# Find required packages
find_package(GTest REQUIRED)
include(GoogleTest)

include(GNUInstallDirs)

add_subdirectory(src)

include(cmake/TestingSetup.cmake)

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

