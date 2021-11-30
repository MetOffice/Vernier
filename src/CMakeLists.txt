# File includes functionality to compile source into a shared library and set
# up installation of the lib and header files upon build and install.
# TODO - Issue #14 - Generalise build for multiple libs

# Include helper functions for creating config files.
include(CMakePackageConfigHelpers)

# Add files source files to library.
add_library(${CMAKE_PROJECT_NAME} SHARED
        profiler.h
        )

# Link library to and external libs (also use project warnings and options).
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        project_warnings
        project_options)

# Set the properties of the library including, the version and language. Also
# set which header files are to be public and installed, this can be useful
# when limiting access to the internal workings of the library with the install.
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        SOVERSION ${SOVERSION}
        VERSION ${VERSION}
        LINKER_LANGUAGE CXX
        PUBLIC_HEADER "profiler.h")

# Set up installation rules for the library.
install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
