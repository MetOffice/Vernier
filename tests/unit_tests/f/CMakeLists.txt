# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
if(BUILD_FORTRAN_TESTS)
    # If build Fortran tests is on add tests
    add_pfunit_ctest( test_vernier_f
            TEST_SOURCES test_vernier_mod.pf
            LINK_LIBRARIES ${CMAKE_PROJECT_NAME}_f
            MAX_PES 2
            )
    message(STATUS "${CMAKE_BINARY_DIR}/src/f/modules")
    target_include_directories(test_vernier_f PUBLIC ${CMAKE_BINARY_DIR}/src/f/modules)
endif()
