# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
#  Locates the PAPI (Performance Application Programming Interface) library.

find_path(PAPI_INCLUDE_DIR
  NAMES papi.h
  HINTS ${PAPI_ROOT} ENV PAPI_DIR ENV PAPI_ROOT
  PATH_SUFFIXES include
)

find_library(PAPI_LIBRARY
  NAMES papi
  HINTS ${PAPI_ROOT} ENV PAPI_DIR ENV PAPI_ROOT
  PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PAPI
  REQUIRED_VARS PAPI_LIBRARY PAPI_INCLUDE_DIR
)

if (PAPI_FOUND)
  set(PAPI_INCLUDE_DIRS "${PAPI_INCLUDE_DIR}")
  set(PAPI_LIBRARIES    "${PAPI_LIBRARY}")
endif()

mark_as_advanced(PAPI_INCLUDE_DIR PAPI_LIBRARY)
