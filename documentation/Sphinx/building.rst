Building
========

Build Options
-------------

There are a number of options which can be passed to CMake on the command line
or set using ccmake. Passing options to CMake via the command line looks 
something like this: ``cmake -DBUILD_FORTRAN_TESTS=OFF -DBUILD_SHARED_LIBS=OFF ..``. 
Alternatively, ``ccmake ..`` will bring up a terminal wherein the user can change 
build options interactively. 

..  list-table::
    :widths: 20 15 30
    :header-rows: 1

    * - Argument
      - Options (Default **Bold**)
      - Description
    * - ``-DBUILD_DOCS``
      - **ON** / OFF
      - Build the documentation. When this is ON, there are individual options
        to disable either Doxygen or Sphinx.
    * - ``-DENABLE_DOXYGEN``
      - **ON** / OFF
      - Enable Doxygen source code documentation generation.
    * - ``-DENABLE_SPHINX``
      - **ON** / OFF
      - Enable Sphinx written documentation generation. 
    * - ``-DBUILD_TESTS``
      - **ON** / OFF
      - Build unit tests.
    * - ``-DBUILD_FORTRAN_TESTS``
      - **ON** / OFF
      - Build Fortran unit tests (requires ``BUILD_TESTS=ON``).
    * - ``-DINCLUDE_GTEST``
      - ON / **OFF**
      - Fetches and populates GoogleTest within the project build (requires 
        ``BUILD_TESTS=ON``).
    * - ``-DBUILD_SHARED_LIBS``
      - **ON** / OFF
      - Determines whether the libraries are linked statically (``OFF``) or 
        dynamically (``ON``).

The table above pertains to options unique to the profiler project. An extensive
list of CMake variables can be found 
`here <https://cmake.org/cmake/help/v3.13/manual/cmake-variables.7.html>`_. 

.. _installation:

Installation
------------

The "install" target will install the profiler's public header files and libraries.

.. code-block:: shell

  mkdir build
  cd build
  cmake ..
  make
  make install

By default the ``include`` and ``lib`` directories will appear in your build 
directory but this can be changed via the ``-DCMAKE_INSTALL_PREFIX`` CMake 
option.

Additional targets
------------------

.. glossary::

   test
     Runs all built unit tests. Only available when ``-DBUILD_TESTS`` is turned
     ON. Identical to running ``ctest``.

   profiler
     Builds just the main profiler target and its associated source code, no
     tests or documentation.

   doxygen
     Uses Doxygen to generate source code documentation based on comment blocks
     in the code. The generated files are put into a ``doxygen`` subdirectory.

   sphinx
     Uses Sphinx to build the written documentation. This target builds the 
     Doxygen documentation first. The generated files are put into a 
     ``sphinx`` subdirectory.
