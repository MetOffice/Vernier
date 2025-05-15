# ------------------------------------------------------------------------------
#  (c) Crown copyright 2025 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------

if (BUILD_FORTRAN_TESTS)
  # Function to simplify adding system-tests
  function(add_fortran_system_test test_name source_file ranks)
    set(TESTNAME_PREFIX "SystemTests")
    message(STATUS "Building ${TESTNAME_PREFIX}.${test_name}")
    add_executable(${TESTNAME_PREFIX}_${test_name} ${source_file})
    target_link_libraries(${TESTNAME_PREFIX}_${test_name} ${CMAKE_PROJECT_NAME}_f)
    target_include_directories(${TESTNAME_PREFIX}_${test_name} PUBLIC ${CMAKE_BINARY_DIR}/src/f/modules)
    add_test(NAME ${TESTNAME_PREFIX}.${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${MPITEST_EXECUTABLE_NAME} ${MPIEXEC_NUMPROC_FLAG} ${ranks} $<TARGET_FILE:${TESTNAME_PREFIX}_${test_name}>
    )
  endfunction()

  if (ENABLE_MPI)
    add_fortran_system_test(TestInitialisation test_initialisation.F90 2)
  endif()
endif ()
