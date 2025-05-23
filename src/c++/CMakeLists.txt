
# ------------------------------------------------------------------------------
#  (c) Crown copyright 2024 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# File includes functionality to compile source into a shared library and set
# up installation of the lib and header files upon build and install.
# TODO - Issue #14 - Generalise build for multiple libs.

# Include helper functions for creating config files.
include(CMakePackageConfigHelpers)

# Add files source files to library. This is STATIC by default and SHARED when
# the BUILD_SHARED_LIBS option is turned ON.
add_library(${CMAKE_PROJECT_NAME}
        vernier.cpp
        hashtable.cpp
        hashvec_handler.cpp
        writer/writer.cpp
        writer/multi.cpp
        formatter.cpp
        hashvec.cpp
        vernier_gettime.cpp
        vernier_get_wtime.cpp
        mpi_context.cpp
        vernier_mpi.cpp
        error_handler.cpp
        )

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADER_FILES vernier.h hashtable.h hashvec.h vernier_gettime.h
          vernier_get_wtime.h vernier_mpi.h mpi_context.h error_handler.h)

# Link library to and external libs (also use project warnings and options).
set (PLIBS OpenMP::OpenMP_CXX)

if (ENABLE_MPI)
  list(APPEND PLIBS MPI::MPI_CXX)
else()
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE USE_VERNIER_MPI_STUB)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${PLIBS})

# Set library C++ standard
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)

set_project_warnings(${CMAKE_PROJECT_NAME})

# Set the properties of the library including, the version and language. Also
# set which header files are to be public and installed, this can be useful
# when limiting access to the internal workings of the library with the install.
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        LINKER_LANGUAGE CXX
        PUBLIC_HEADER "${PUBLIC_HEADER_FILES}")

# Set up installation rules for the library.
install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
