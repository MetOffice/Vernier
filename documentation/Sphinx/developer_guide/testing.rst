.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

.. _testing:

Testing
=======

Tests are in the repository
`here <https://github.com/MetOffice/Vernier/tree/main/tests>`__.

Unit Tests
----------

The C++ and Fortran unit tests utilise the GoogleTest and pFUnit frameworks,
respectively. The unit tests are stored in the ``tests/unit_tests`` directory


System Tests
------------

The system tests for Vernier are standalone programs with no test runner. The
system tests are stored in the ``tests/system_tests`` directory.

Running the tests
-----------------
All the tests are added to the CMake build system and can be run using the
``ctest`` command. The tests are run in the build directory, which the user
must create when building Vernier. The tests can be run using the following
commands:

.. code-block:: shell

   mkdir build
   cd build
   cmake ..
   make
   # Either 'make test' or 'ctest'.
   make test