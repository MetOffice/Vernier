# This function will prevent in-source builds
function(AssureOutOfSourceBuilds)
    # Make sure the user doesn't perform in-source builds by tricking
    # the build system through the use of symlinks
    get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
    get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

    # Disallow in-source builds
    if ("${srcdir}" STREQUAL "${bindir}")
        message("######################################################")
        message("Warning: in-source builds are disabled")
        message("Please create a separate build directory and run cmake from there")
        message("######################################################")
        message(FATAL_ERROR "Quitting configuration")
    endif ()
endfunction()

AssureOutOfSourceBuilds()
