# This sets up the testing framework inclusion for the project, optionally
# googletest can be downloaded from the git repository and built in the main
# build directory for use by ctest. By default cmake will use find_package to
# locate the installed library.

# Optional argument to build the tests
option(BUILD_TESTS "Build the tests against GoogleTest" ON)

if(BUILD_TESTS)
    # Add optional argument to fetch GoogleTest
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
        message(STATUS "Using local GoogleTest installation")

        # Find installed GoogleTest library.
        find_package(GTest CONFIG REQUIRED)
    endif ()

    # Include googletest testing library in project.
    include(GoogleTest)

    # Enable testing with CTest
    enable_testing()
    add_subdirectory(tests)
endif()
