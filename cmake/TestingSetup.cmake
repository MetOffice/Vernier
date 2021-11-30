# Enable CTest integration with GoogleTest and add a directory for the tests

# Find required testing packages
find_package(GTest REQUIRED)
include(GoogleTest)

# Enable CTest
enable_testing()

add_subdirectory(tests)
