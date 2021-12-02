option(INCLUDE_GTEST "Fetch GoogleTest framework during build" OFF)

if (INCLUDE_GTEST)
    message(STATUS "Fetching GoogleTest")
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG	e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
    set(TESTING_LIB gtest_main)
else()
    message(STATUS "Using local GoogleTest installation")
    find_package(GTest REQUIRED)
    set(TESTING_LIB GTest::Main)
endif ()
include(GoogleTest)
enable_testing()
add_subdirectory(tests)
