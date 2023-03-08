User Manual
===========

Setting up
----------

Manually linking to installed libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instructions on how to install [name]'s libraries and public header
files can be found :ref:`here <installation>`. The ``lib`` directory contains
libraries to be linked against, whereas the ``include`` directory contains any
public header files (C++) or modules (Fortran) that need to be included to gain
access to the API.

First, the path to the installed libraries will need to be added to
``LD_LIBRARY_PATH``.

.. code-block:: shell

   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/lib

The following flags can then be used at the compile step in order to use
profiler calls in your code:

.. code-block:: shell

   # when compiling C++ code
   -I/path/to/include -L/path/to/lib -lprofiler

.. code-block:: shell

   # when compiling Fortran code
   -I/path/to/include -L/path/to/lib -lprofiler_f -lprofiler_c -lprofiler

For example, the full process of compiling and running a fortran program called
``example.f90`` may look something like this:

.. code-block:: shell

   mpifort -fc=clang++ example.f90 -I./include -L./lib64 -lprofiler_f -lprofiler_c -lstdc++ -fopenmp
   mpirun -n 6 ./a.out

Including profiler in a CMake project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TBD

API
---

The profiler's API consists of three primary functions; **start**, **stop**,
and **write**. A timed region is defined by a start-stop pair.

How calls to these functions are made differs slightly between C++ and Fortran,
but the core functionality is the same. 

In both cases, a region name must be provided that will appear in the final 
output table. Also, a variable must be declared that [name] can use to store
the associated region's hash.

This hash variable is of type ``size_t`` in C++. In Fortran, the ``profiler_mod``
module introduces a new integer kind for these: ``pik`` (*profiler* *integer*
*kind*).

C++
^^^

.. code-block:: cpp
 
   #include "profiler.h"

   // Start
   auto prof_handle = prof.start("Main region");

   // Stop
   prof.stop(prof_handle);

   // Write
   prof.write();

Fortran
^^^^^^^

.. code-block:: f90

   use profiler_mod
   integer (kind=pik) :: prof_handle

   ! Start
   call profiler_start(prof_handle, "Main region")

   ! Stop
   call profiler_stop(prof_handle)

   ! Write
   call profiler_write()

Dos and don'ts
^^^^^^^^^^^^^^

**Do**:

* Initialise MPI before profiling
* Nest timed regions nicely (no overlap)

**Don't**:

* Add milk to your bowl before cereal

Example API Usage
^^^^^^^^^^^^^^^^^

More fleshed out examples can be seen in the ``profiler/tests/system_tests`` directory.

Interpreting Output
-------------------

By default [name] will write information out into one file per MPI rank.
Single file output is not currently supported but is achievable via
post-processing techniques.

Each file contains a table of the all the declared regions and their
associated walltimes. 

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

Example Outputs
^^^^^^^^^^^^^^^

TODO
