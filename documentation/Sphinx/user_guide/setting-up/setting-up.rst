.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

.. _settingup:

Setting Up
----------

Instructions on how to install Vernier's libraries and public header
files can be found :ref:`here <installation>`. The ``lib`` directory contains
libraries to be linked against, whereas the ``include`` directory contains any
public header files (C++) or modules (Fortran) that need to be included to gain
access to the API.

The ``-DBUILD_SHARED_LIBS`` CMake option is used to choose whether the libraries
are built statically or dynamically.

Manually Linking to Installed Libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Flags of interest when compiling manually:

.. code-block:: shell

   # C++
   mpicxx foo.cpp -I/path/to/include -Wl,-rpath=/path/to/lib -L/path/to/lib -lvernier

   # Fortran
   mpifort bar.f90 -I/path/to/include -Wl,-rpath=/path/to/lib -L/path/to/lib -lvernier_f -lvernier_c -lvernier

It is also possible to use ``LD_LIBRARY_PATH`` instead of ``-Wl,-rpath=``:

.. code-block:: shell

   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/lib

If Vernier has been built without MPI support and is being used with
C++, it can be compiled using the standard compiler and the
`USE_VERNIER_MPI_STUB` pre-processor directive.  For example:

.. code-block:: shell

   # C++
   c++ foo.cpp -DUSE_VERNIER_MPI_STUB -I/path/to/include -Wl,-rpath=/path/to/lib -L/path/to/lib -lvernier

Including Vernier in a CMake Project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The CMake lines below are always a necessary starting point in order to find
the Vernier package. This can be done anywhere in your project directory as
long as Vernier is found *before* linking/using.

.. code-block:: cmake

   # The 'find_package' function needs to know where to go looking for the
   # Vernier package
   set(CMAKE_PREFIX_PATH /path/to/lib)

   # Find Vernier
   find_package(vernier CONFIG)

.. note::

   Instead of setting ``CMAKE_PREFIX_PATH``, you can also do...

   .. code-block:: cmake

      set(vernier_DIR /path/to/lib/cmake/vernier)

   (*The key difference being that the full path to* ``vernierConfig.cmake`` *is
   mandatory, whereas it is sufficient to just point to the* ``lib`` *directory
   when using* ``CMAKE_PREFIX_PATH``).

The subsequent steps for linking to the Vernier libraries and adding include
dependencies may look something like:

.. code-block:: cmake

   # Link Vernier libraries 
   target_link_libraries(foo PUBLIC
           vernier::vernier_f  # Only required for Fortran
           vernier::vernier_c  # Only required for Fortran
           vernier::vernier
           )

   # Include Vernier headers 
   target_include_directories(foo PUBLIC
           /path/to/include
           )

.. _env-variables:

Environment Variables
^^^^^^^^^^^^^^^^^^^^^

.. glossary::

   ``VERNIER_OUTPUT_FORMAT``

     This environment variable determines the format of the outputted tables of
     data. There are currently two options:

     * **drhook**: Mimics the output format of the DrHook profiling tool so 
       that the same post-processing techniques can be used.

     * **threads**: A custom, strung-together, format where threads have
       their own seperate table of walltimes.

     If this environment variable remains unset, then the default output format
     is the **drhook** option.

   ``VERNIER_OUTPUT_MODE``

     Determines the output mode to use. Currently supported values are:

     * **multi**: Every task writes to its own output file
       
     * **single**: All tasks write to a single global output file

     If the environment variable is not set, **multi** mode is used.

   ``VERNIER_OUTPUT_FILENAME``

     Sets the output filename, which is ``vernier-output`` by
     default.

     Vernier will append the MPI rank onto the end of this
     name when running in **multi** mode, resulting in a file called
     `vernier-output-0` for the first MPI rank, for example.

     Vernier will append ``-global`` to the name when running in
     **single** mode and the file will contain formatted entries for
     each task ordered by MPI rank.
