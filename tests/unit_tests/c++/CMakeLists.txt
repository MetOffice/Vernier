# Function to simplify adding unit-tests
function(add_unit_test test_name cpp_file)
    add_executable(${test_name} ${cpp_file})
    target_link_libraries(${test_name}
            OpenMP::OpenMP_CXX
            GTest::gtest_main
            ${CMAKE_PROJECT_NAME})
    set_project_warnings(${test_name})
    target_include_directories(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/src/c++)
    gtest_discover_tests(${test_name})
endfunction()

# List of unit-tests added to CTest. Add a line calling the 'add_unit_test'
# function to add an additional file of tests.
add_unit_test(test_profiler test_profiler.cpp) 
add_unit_test(test_hashtable test_hashtable.cpp)
