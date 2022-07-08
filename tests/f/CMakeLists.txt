add_pfunit_ctest( test_profiler_f
        TEST_SOURCES test_profiler_mod.pf
        LINK_LIBRARIES ${CMAKE_PROJECT_NAME}_f)
message(STATUS "${CMAKE_BINARY_DIR}/src/f/modules")
target_include_directories(test_profiler_f PUBLIC ${CMAKE_BINARY_DIR}/src/f/modules)