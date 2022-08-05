# This file sets up the options for the Doxygen generation of API documentation.
# Available options for CMake Doxygen integration can be found here:
# https://cmake.org/cmake/help/latest/module/FindDoxygen.html
function(enable_doxygen)
    option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" ON)
    if (ENABLE_DOXYGEN)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_PRIVATE YES)
        set(DOXYGEN_GENERATE_LATEX YES)
        set(DOXYGEN_LATEX_OUTPUT pdf)
        set(DOXYGEN_MARKDOWN_SUPPORT YES)
        set(DOXYGEN_HIDE_SCOPE_NAMES YES)
        set(DOXYGEN_PROJECT_NAME "${CMAKE_PROJECT_NAME}")
        set(DOXYGEN_PROJECT_NUMBER "v${PROJECT_VERSION}")
        set(DOXYGEN_DISABLE_INDEX NO)
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_JAVADOC_BANNER NO)
        set(DOXYGEN_JAVADOC_AUTOBRIEF NO)
        set(DOXYGEN_JAVADOC_BLOCK NO)
        set(DOXYGEN_FULL_PATH_NAMES NO)
        set(DOXYGEN_STRIP_CODE_COMMENTS NO)
        set(DOXYGEN_FILE_PATTERNS  *.c *.cpp *.h *.f90 *.F90 )
        set(DOXYGEN_EXTENSION_MAPPING "F90=Fortran")
        set(DOXYGEN_HTML_HEADER ${PROJECT_SOURCE_DIR}/documentation/Doxygen/html/header.html)
        set(DOXYGEN_HTML_FOOTER ${PROJECT_SOURCE_DIR}/documentation/Doxygen/html/footer.html)

        # Find the Doxygen application install (requires "dot" tool).
        find_package(Doxygen REQUIRED dot)

        # Set up Doxygen build with source directory and additional pages.
        doxygen_add_docs(doxygen-docs ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/documentation/Doxygen ALL)

        # Set up installation destination.
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/docs)
    endif ()
endfunction()
