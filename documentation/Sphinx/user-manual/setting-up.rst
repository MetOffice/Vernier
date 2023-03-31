.. _settingup:

Setting Up
----------

Instructions on how to install profler's libraries and public header
files can be found :ref:`here <installation>`. The ``lib`` directory contains
libraries to be linked against, whereas the ``include`` directory contains any
public header files (C++) or modules (Fortran) that need to be included to gain
access to the API.

The ``-DBUILD_SHARED_LIBS`` CMake option is used to choose whether the libraries
are built statically or dynamically.

Manually linking to installed libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Flags of interest when compiling manually:

.. code-block:: shell

   # C++
   mpicxx foo.cpp -I/path/to/include -Wl,-rpath=/path/to/lib -L/path/to/lib -lprofiler

   # Fortran
   mpifort bar.f90 -I/path/to/include -Wl,-rpath=/path/to/lib -L/path/to/lib -lprofiler_f -lprofiler_c -lprofiler

It's also possible to use ``LD_LIBRARY_PATH`` instead of ``-Wl,-rpath=``:

.. code-block:: shell

   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/lib

Including profiler in a CMake project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The CMake lines below are always a necessary starting point in order to find
the profiler package. This can be done anywhere in your project directory as
long as profiler is found *before* linking/using.

.. code-block:: cmake

   # The find_package function needs to know where to go looking for the
   # profiler package
   set(CMAKE_PREFIX_PATH /path/to/lib)

   # Find profiler
   find_package(profiler CONFIG)

.. note::

   Instead of setting ``CMAKE_PREFIX_PATH``, you can also do...

   .. code-block:: cmake

      set(profiler_DIR /path/to/lib/cmake/profiler)

   (*The key difference being that the full path to* ``profilerConfig.cmake`` *is
   mandatory, whereas it is sufficient to just point to the* ``lib`` *directory
   when using* ``CMAKE_PREFIX_PATH``).

The subsequent steps for linking to the profiler libraries and adding include
dependencies may look something like:

.. code-block:: cmake

   # Link profiler libraries 
   target_link_libraries(foo PUBLIC
           profiler::profiler_f  # Only required for Fortran
           profiler::profiler_c  # Only required for Fortran
           profiler::profiler
           )

   # Include profiler headers 
   target_include_directories(foo PUBLIC
           /path/to/include
           )

.. _env-variables:

Environment Variables
^^^^^^^^^^^^^^^^^^^^^

.. glossary::

   ``PROF_OUTPUT_FORMAT``

     This environment variable determines the format of the outputted tables of
     data. There are currently two options:

     * **drhook**: Mimics the output format of the DrHook profiling tool so 
       that the same post-processing techniques can be used.

     * **threads**: A custom, strung-together, format where threads have
       their own seperate table of walltimes.

     If this environment variable remains unset, then the default output format
     is the **drhook** option.

   ``PROF_IO_MODE``

     Determines the output mode to use. Currently only supports being set to 
     **multi** but single-file-output may be added in the future.

   ``PROF_OUTPUT_FILENAME``

     Sets the output filename, which is "profiler-output" by default. profler
     will append the MPI rank onto the end of this name by default, resulting
     in a file called "profiler-output-0" for the first MPI rank, for example.
