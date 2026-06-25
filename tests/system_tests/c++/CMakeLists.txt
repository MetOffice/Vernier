# ------------------------------------------------------------------------------
#  (c) Crown copyright 2025 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------

if (BUILD_TESTS)
    add_custom_target(copy_data
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/test_data
                ${CMAKE_CURRENT_BINARY_DIR}/test_data
            COMMENT "Copying system test data files to build directory"
    )
    set(COPY_DATA_DEPS)
    #
    # Add MPI to parallel libraries if required.
    #

    set(PLIBS OpenMP::OpenMP_CXX)

    if (ENABLE_MPI)
        list(APPEND PLIBS MPI::MPI_CXX)
    endif ()

    # Function to simplify adding system-tests
    function(add_cxx_system_test test_name source_file ranks copy_data)
        set(TESTNAME_PREFIX "SystemTests")
        message(STATUS "Building ${TESTNAME_PREFIX}.${test_name}")
        add_executable(${TESTNAME_PREFIX}_${test_name} ${source_file})
        target_link_libraries(${TESTNAME_PREFIX}_${test_name} ${CMAKE_PROJECT_NAME} ${PLIBS})
        target_include_directories(${TESTNAME_PREFIX}_${test_name} PUBLIC ${CMAKE_BINARY_DIR}/src/c++)
        if (NOT ENABLE_MPI)
            target_compile_definitions(${TESTNAME_PREFIX}_${test_name} PRIVATE USE_VERNIER_MPI_STUB)
        endif ()
        set_project_warnings(${TESTNAME_PREFIX}_${test_name})
        add_test(NAME ${TESTNAME_PREFIX}_${test_name}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND ${MPITEST_EXECUTABLE_NAME} ${MPIEXEC_NUMPROC_FLAG} ${ranks} $<TARGET_FILE:${TESTNAME_PREFIX}_${test_name}>
        )
        if (copy_data)
            set(COPY_DATA_DEPS
                "${COPY_DATA_DEPS};${TESTNAME_PREFIX}_${test_name}"
                PARENT_SCOPE)
        endif ()
    endfunction()

    add_cxx_system_test(TestTags test-tags.cpp 1 FALSE)
    add_cxx_system_test(TestOutputs test-output-files.cpp 1 TRUE)

    if (ENABLE_MPI)
        add_cxx_system_test(TestVernierMPIHeaders test-vernier-mpi-headers.cpp 2 TRUE)
        add_cxx_system_test(TestMPIOutputs test-output-files.cpp 2 TRUE)
    endif ()
    add_dependencies(${COPY_DATA_DEPS} copy_data)
    endif ()
