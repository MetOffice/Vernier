Build Guide
===========

.. _requirements:

Requirements
------------

The tested compilers are:

* GCC 9.3.0, 10.2.0
* Clang 12.0

Requirements for building:

* C++17 compatible compiler.
* CMake 3.13
* MPICH 3.4.1
* OpenMP 4.5 (if applicable)

For testing and documenation:

* GoogleTest 1.11.0
* pFUnit 4.4.1
* Doxygen 1.8.5
* Sphinx 4.3.1
  
  * docutils 0.16 (rendering issues are known to exist in newer versions!)
  
* Breathe 4.34.0

.. note::
   
   Unless stated otherwise versions are a *minimum*. Newer versions
   should be backwards compatible but haven't necessarily been tested.

Building & Options
------------------

To get started, checkout the Vernier repository:

.. code-block:: shell

   git clone git@github.com:MetOffice/Vernier.git
   cd Vernier

It's also recommended that a build directory is setup, as in-source builds are
not supported. After this, CMake can then be used to perform a full build.

.. code-block:: shell

   mkdir build
   cd build
   cmake ..
   make

There are a number of options which can be passed to CMake on the command line
or set using ``ccmake``. The table below highlights some of the flags that can
be passed to CMake on the command line. Alternatively, ``ccmake ..`` will bring
up a terminal wherein the user can change build options interactively. 

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

The table above pertains to options specific to Vernier. An extensive
list of CMake internal variables can be found 
`here <https://cmake.org/cmake/help/v3.13/manual/cmake-variables.7.html>`_.

.. _installation:

Installation
------------

The "install" target will install Vernier's public header files and libraries.
After navigating to your build directory:

.. code-block:: shell

   cmake ..
   make install

**The default installation location is in** ``Vernier/build`` **but this can be
changed via the** ``-DCMAKE_INSTALL_PREFIX`` **CMake option**.

The next page (the :ref:`user manual<settingup>`) describes how to link the
installed libraries to your program.

Additional Targets
------------------

Additional targets that can be built from the command line using `make`.

.. glossary::

   `test`
     Runs all built unit tests. Only available when ``-DBUILD_TESTS`` is turned
     ON. Identical to running ``ctest``.

   `vernier`
     Builds just the main project target and its associated source code, no
     tests or documentation.

   `doxygendocs`
     Uses Doxygen to generate source code documentation based on comment blocks
     in the code. The generated files are put into a ``doxygen`` subdirectory.

   `sphinxdocs`
     Uses Sphinx to build the written documentation. This target builds the 
     Doxygen documentation first. The generated files are put into a 
     ``sphinx`` subdirectory.
