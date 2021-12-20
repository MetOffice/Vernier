# Function to simplify adding unit-tests
function(add_unit_test test_name cpp_file)
    add_executable(${test_name} ${cpp_file})
    target_link_libraries(${test_name}
            project_warnings
            project_options
            GTest::gtest_main)
    target_include_directories(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/src)
    gtest_discover_tests(${test_name})
endfunction()

# List of unit-tests added to CTest. Add a line calling the 'add_unit_test'
# function to add an additional file of tests.
add_unit_test(hello_test hello_test.cpp)
