# ------------------------------------------------------------------------------
#  (c) Crown copyright 2024 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
if (BUILD_FORTRAN_TESTS)
  # Function to simplify adding unit-tests
  function(add_fortran_unit_test test_name pf_file)
    set(TESTNAME_PREFIX "FortranAPITests")
    message(STATUS "Building ${test_name}")
    add_pfunit_ctest(${TESTNAME_PREFIX}.${test_name}
        TEST_SOURCES ${pf_file}
        LINK_LIBRARIES ${CMAKE_PROJECT_NAME}_f
        MAX_PES 2
    )
    target_include_directories(${TESTNAME_PREFIX}.${test_name} PUBLIC ${CMAKE_BINARY_DIR}/src/f/modules)
    if (ENABLE_MPI)
      target_compile_definitions(${TESTNAME_PREFIX}.${test_name} PRIVATE USE_MPI)
    endif()
  endfunction()

  if (ENABLE_MPI)
      add_fortran_unit_test(TestVernier test_vernier_mod.pf)
  endif()

endif ()
