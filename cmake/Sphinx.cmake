# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
if(ENABLE_SPHINX)
    # Add the cmake folder so the FindSphinx module is found
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    
    # Find Sphinx
    find_package(Sphinx REQUIRED)
    if(SPHINX_FOUND)
        # Sphinx options
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/documentation/Sphinx)
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
        set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
        # Custom sphinx target
        add_custom_target(sphinx ALL
                            COMMAND ${SPHINX_EXECUTABLE} -b html
                                    -Dbreathe_projects.profler=${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml
                                    ${SPHINX_SOURCE} ${SPHINX_BUILD}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                            COMMENT "Generating docs with Sphinx"
                            VERBATIM)
    endif()
else()
    if(BUILD_DOCS)
        message(STATUS "ENABLE_SPHINX disabled, skipping Sphinx setup.")
    endif()
endif()

