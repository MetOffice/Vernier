# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
if(ENABLE_PAPI)
    # Add the cmake folder so the FindPAPI module is found
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    
    find_package(PAPI REQUIRED)
else()
    message(STATUS "ENABLE_PAPI disabled.")
endif()

