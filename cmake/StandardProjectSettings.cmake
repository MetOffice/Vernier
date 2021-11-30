# Some basic project settings for defaults such as build type and installation
# prefix.

# Set C++ standard compliance.
set(CMAKE_CXX_STANDARD 17)

# Set a default build type if none was specified.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE
            RelWithDebInfo
            CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui, ccmake.
    set_property(
            CACHE CMAKE_BUILD_TYPE
            PROPERTY STRINGS
            # These build type options set the listed compiler options.
            # These are CMake default flags for C++.
            "Debug" # -g
            "Release" # -O3 -DNDEBUG
            "MinSizeRel" # -Os -DNDEBUG
            "RelWithDebInfo" # -O2 -g -DNDEBUG
    )
endif ()

# Generate compile_commands.json to make it easier to work with Clang-based tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Interprocedural optimisation is useful for improving program performance through
# analysis of the entire program rather than just single program units.
option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)

if (ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(
            RESULT
            result
            OUTPUT
            output)
    if (result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(SEND_ERROR "IPO is not supported: ${output}")
    endif ()
endif ()

# Set default install location to be in build directory rather than /usr/bin etc.
if (${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "..." FORCE)
endif ()
