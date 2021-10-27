include(CMakePackageConfigHelpers)

add_library(${CMAKE_PROJECT_NAME} SHARED
        profiler.h
        )

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        project_warnings
        project_options)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        SOVERSION ${SOVERSION}
        VERSION ${VERSION}
        LINKER_LANGUAGE CXX
        PUBLIC_HEADER "profiler.h")

install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT  "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
