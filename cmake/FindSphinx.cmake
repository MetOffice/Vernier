# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# Find Sphinx executable
include(FindPackageHandleStandardArgs)
find_program(SPHINX_EXECUTABLE
            NAMES sphinx-build sphinx-build.exe 
            DOC "Path to sphinx executable")
mark_as_advanced(SPHINX_EXECUTABLE)
find_package_handle_standard_args(Sphinx "Error locating sphinx executable" SPHINX_EXECUTABLE)