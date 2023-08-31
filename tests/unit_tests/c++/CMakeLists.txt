# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------

#
# MPI initialised.
#

# Function to simplify adding unit-tests
function(add_unit_test test_name cpp_file)
    add_executable(${test_name} ${cpp_file} main.cpp)
    target_link_libraries(${test_name}
            OpenMP::OpenMP_CXX
            GTest::gtest_main
            GTest::gmock_main
            MPI::MPI_CXX
            ${CMAKE_PROJECT_NAME})
    set_project_warnings(${test_name})
    target_include_directories(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/src/c++)
    gtest_discover_tests(${test_name})
endfunction()

# List of unit-tests added to CTest. Add a line calling the 'add_unit_test'
# function to add an additional file of tests.
add_unit_test(test_profiler test_profiler.cpp)
add_unit_test(test_hashtiming test_hashtiming.cpp)
add_unit_test(test_regionname test_regionname.cpp)
add_unit_test(test_hashtable test_hashtable.cpp)
add_unit_test(test_proftests test_proftests.cpp)
add_unit_test(test_callcount test_callcount.cpp)
add_unit_test(test_recursion test_recursion.cpp)

#
# Test behaviour when MPI is not initialised.
#

# Function to simplify adding unit-tests
function(add_serial_unit_test test_name cpp_file)
    add_executable(${test_name} ${cpp_file})
    target_link_libraries(${test_name}
            OpenMP::OpenMP_CXX
            GTest::gtest_main
            GTest::gmock_main
            MPI::MPI_CXX
            ${CMAKE_PROJECT_NAME})
    set_project_warnings(${test_name})
    target_include_directories(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/src/c++)
    gtest_discover_tests(${test_name})
endfunction()

add_serial_unit_test(test_serial test_serial.cpp)

