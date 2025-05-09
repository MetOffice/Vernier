# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# This sets up the testing framework inclusion for the project, optionally
# googletest can be downloaded from the git repository and built in the main
# build directory for use by ctest. By default cmake will use find_package to
# locate the installed library.

# Optional argument to build the tests.
option(BUILD_TESTS "Build the tests against GoogleTest" ON)


if(BUILD_TESTS)
    # Add optional argument to fetch GoogleTest.
    option(INCLUDE_GTEST "Fetch GoogleTest framework during build" OFF)

    if (INCLUDE_GTEST)
        message(STATUS "Fetching GoogleTest")

        # Include FetchContent module to enable populating content at configure time.
        include(FetchContent)

        # Set up GoogleTest options to populate lib.
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG	e2239ee6043f73722e7aa812a459f54a28552929 # Hash of release-1.11.0.
        )

        # Get the properties of the content being populated (e.g. googletest_BINARY_DIR).
        FetchContent_GetProperties(googletest)

        # Populate content but don't allow installation with project library.
        if(NOT googletest_POPULATED)
            FetchContent_Populate(googletest)
            add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()
    else()

        # Find installed GoogleTest library.
        find_package(GTest 1.11.0 CONFIG REQUIRED)

        message(STATUS "Found GoogleTest testing framework")
    endif ()

    # Include googletest testing library in project.
    include(GoogleTest)

    # Turn on testing of Fortran interface
    option(BUILD_FORTRAN_TESTS "Build Fortran tests against pFUnit" ON)

    # Find pFUnit
    if(BUILD_FORTRAN_TESTS)
        find_package(PFUNIT 4.4.1 REQUIRED)
        message(STATUS "Found pFUnit testing framework")
    endif()

    # Set the MPI test executable name.
    set(MPITEST_EXECUTABLE_NAME mpiexec)
    set(MPIEXEC_NUMPROC_FLAG "-np")

    # Enable testing with CTest.
    enable_testing()
    add_subdirectory(tests)

endif()
