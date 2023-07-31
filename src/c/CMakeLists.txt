# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# Include helper functions for creating config files.
include(CMakePackageConfigHelpers)

# Add files source files to library.
add_library(${CMAKE_PROJECT_NAME}_c 
        profiler_c.cpp)

# Link library to and external libs (also use project warnings and options).
target_link_libraries(${CMAKE_PROJECT_NAME}_c PRIVATE
                        OpenMP::OpenMP_CXX
                        ${CMAKE_PROJECT_NAME}
                     )

set_project_warnings(${CMAKE_PROJECT_NAME}_c)

target_include_directories(${CMAKE_PROJECT_NAME}_c PRIVATE
        ${PROJECT_SOURCE_DIR}/src/c++)

# Set the properties of the library including, the version and language. Also
# set which header files are to be public and installed, this can be useful
# when limiting access to the internal workings of the library with the install.
set_target_properties(${CMAKE_PROJECT_NAME}_c PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        LINKER_LANGUAGE CXX)

# Set up installation rules for the library.
install(TARGETS ${CMAKE_PROJECT_NAME}_c EXPORT "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
