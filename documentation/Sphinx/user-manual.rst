User Manual
===========

Setting Up
----------

Manually linking to installed libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instructions on how to install [name]'s libraries and public header
files can be found :ref:`here <installation>`. The ``lib`` directory contains
libraries to be linked against, whereas the ``include`` directory contains any
public header files (C++) or modules (Fortran) that need to be included to gain
access to the API.

The ``-DBUILD_SHARED_LIBS`` CMake option is used to choose whether the libraries
are built statically or dynamically.

**Linking via LD_LIBRARY_PATH**:

.. code-block:: shell

   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
   mpicxx foo.cpp -L./lib -I./include -lprofiler

**Linking via rpath**: (recommended)

.. code-block:: shell

   mpicxx foo.cpp -Wl,rpath=./lib -L./lib -I./include -lprofiler

.. note:: 

   The ``-lprofiler_f -lprofiler_c`` flags are also needed when compiling
   fortran code.

Including profiler in a CMake project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO

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

     Sets the output filename, which is "profiler-output" by default. [name]
     will append the MPI rank onto the end of this name by default, resulting
     in a file called "profiler-output-0" for the first MPI rank, for example.

API
---

The profiler's API consists of three primary functions; **start**, **stop**,
and **write**. A timed region is defined by a start-stop pair.

How calls to these functions are made differs slightly between C++ and Fortran,
but the core functionality is the same. 

C++
^^^

.. doxygenfunction:: Profiler::start
   :project: profiler

.. doxygenfunction:: Profiler::stop
   :project: profiler

.. doxygenfunction:: Profiler::write
   :project: profiler

Fortran
^^^^^^^

.. doxygennamespace:: profiler_mod
   :project: profiler
   :content-only:

.. doxygeninterface:: profiler_mod::profiler_stop

.. doxygeninterface:: profiler_mod::profiler_write


Dos and don'ts
^^^^^^^^^^^^^^

**Do**:

* Initialise MPI before profiling.
* Nest timed regions nicely (no overlap).

**Don't**:

* Use a singular hash (or "handle") for all regions.
* Have a stop calliper after any ``return`` statements.

Examples
^^^^^^^^

**C++**:

.. code-block:: cpp
 
   #include "profiler.h"

   // Start
   auto prof_handle = prof.start("Main region");

   // Stop
   prof.stop(prof_handle);

   // Write
   prof.write();

**Fortran**:

.. code-block:: f90

   use profiler_mod
   integer (kind=pik) :: prof_handle

   ! Start
   call profiler_start(prof_handle, "Main region")

   ! Stop
   call profiler_stop(prof_handle)

   ! Write
   call profiler_write()

More fleshed out examples can be seen in the ``profiler/tests/system_tests``
directory.

Interpreting Output
-------------------

By default [name] will write information out into one file per MPI rank.
Single file output is not currently supported but is achievable via
post-processing techniques.

Each file contains a table of the all the declared regions and their
associated walltimes. 
