# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# Contains a function for setting up the appropriate warnings for
# different compilers and build settings.
function(set_project_warnings project_name)
    # Create option for forcing errors for all warnings.
    option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

    # Create option to turn on sanitizers.
    option(USE_SANITIZERS "Turn on sanitizers to help reporting of runtime errors" OFF)

    set(CLANG_WARNINGS
            # All warnings (actually not all warnings obviously)
            -Wall
            # More warnings (should be used on non-legacy code)
            -Wextra
            # Warn the user if a variable declaration shadows one from a parent context
            -Wshadow
            # Warn the user if a class with virtual functions has a non-virtual destructor.
            # This helps catch hard to track down memory errors
            -Wnon-virtual-dtor
            # Warn for C-style casts
            -Wold-style-cast
            # Warn for potential performance problem casts
            -Wcast-align
            # Warn on anything being unused
            -Wunused
            # Warn if you overload (not override) a virtual function
            -Woverloaded-virtual
            # Warn if non-standard C++ is used
            -Wpedantic
            # Warn on type conversions that may lose data
            -Wconversion
            # Warn on sign conversions
            -Wsign-conversion
            # Warn if a null dereference is detected
            -Wnull-dereference
            # Warn if float is implicit promoted to double
            -Wdouble-promotion
            # Warn on security issues around functions that format output (i.e. 'printf')
            -Wformat=2
            )

    # Set warnings as errors if requested (recommended during development/debug)
    if (WARNINGS_AS_ERRORS)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
    endif ()

    set(GCC_WARNINGS
            # Include all Clang compiler warnings (uses same flags as g++)
            ${CLANG_WARNINGS}
            # Warn if indentation implies blocks where blocks do not exist
            -Wmisleading-indentation
            # Warn if 'if / else' chain has duplicated conditions
            -Wduplicated-cond
            # Warn if 'if / else' branches have duplicated code
            -Wduplicated-branches
            # Warn about logical operations being used where bitwise were probably wanted
            -Wlogical-op
            # Warn if you perform a cast to the same type
            -Wuseless-cast
            )

    # Set compiler and linker flags depending on compiler
    if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        # TODO Clang sanitizers need attention.
        if(USE_SANITIZERS)
            # Set Clang sanitizers for memory errors/leaks and undefined behaviour
            # set(CLANG_WARNINGS -fsanitize=address,undefined ${CLANG_WARNINGS})
            # set(CLANG_LINKER_FLAGS -fsanitize=address,undefined)
            set(CLANG_WARNINGS ${CLANG_WARNINGS})
        endif()
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
        set(LINKER_FLAGS ${CLANG_LINKER_FLAGS})
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(USE_SANITIZERS)
            # Get GCC sanitizers for memory errors/leaks, undefined behaviour and pointer operands
            set(GCC_WARNINGS
                    -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract
                    ${GCC_WARNINGS})
            set(GCC_LINKER_FLAGS
                    -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract)
        endif()
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
        set(LINKER_FLAGS ${GCC_LINKER_FLAGS})
    else ()
        message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif ()

    # Add compiler and linker flags to project_warnings
    target_compile_options(${project_name} PRIVATE ${PROJECT_WARNINGS})
    target_link_options(${project_name} PRIVATE ${LINKER_FLAGS})

endfunction()
