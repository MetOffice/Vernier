set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

# Testing for building a fortran lib and executable.
add_library(profiler_f SHARED "profiler_mod.F90")
target_link_libraries(profiler_f PUBLIC OpenMP::OpenMP_Fortran profiler_c)

add_executable(fortran_test.x "fortran_test.F90")
target_link_libraries(fortran_test.x profiler_f)

include(FortranCInterface)
FortranCInterface_VERIFY()
FortranCInterface_VERIFY(CXX)
FortranCInterface_HEADER(FC.h MACRO_NAMESPACE "FC_")

# Set the properties of the library including, the version and language. Also
# set which header files are to be public and installed, this can be useful
# when limiting access to the internal workings of the library with the install.
set_target_properties(${CMAKE_PROJECT_NAME}_f PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        LINKER_LANGUAGE Fortran)

# Set up installation rules for the library.
install(TARGETS ${CMAKE_PROJECT_NAME}_f EXPORT "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})