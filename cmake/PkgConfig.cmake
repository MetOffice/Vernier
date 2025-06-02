# ------------------------------------------------------------------------------
#  (c) Crown copyright 2025 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------

if(ENABLE_PKGCONFIG)
  # Create and install a pkg-config file
  message(STATUS "Generating a pkg-config file")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${PROJECT_NAME}.pc
                 @ONLY)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/${PROJECT_NAME}.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
