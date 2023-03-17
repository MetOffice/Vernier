# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# This file sets up the options for the Doxygen generation of API documentation.
# Available options for CMake Doxygen integration can be found here:
# https://cmake.org/cmake/help/latest/module/FindDoxygen.html
if(ENABLE_DOXYGEN)
    # Find the Doxygen application install (requires "dot" tool).
    find_package(Doxygen REQUIRED dot)
    if(DOXYGEN_FOUND)
        # Doxygen options
        set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
        set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/documentation/Doxygen/Doxyfile.in)
        set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen/Doxyfile.out)
        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
        # Custom doxygen target
        add_custom_target(doxygendocs
                          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                          COMMENT "Generating Doxygen source code documentation"
                          VERBATIM)
    endif()
else()
    if(BUILD_DOCS)
        message(STATUS "ENABLE_DOXYGEN disabled, skipping Doxygen setup")
    endif()
endif()

