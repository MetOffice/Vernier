Usage Guide
===========

Adding profiler to your program
-------------------------------

Manually linking to installed libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instructions on how to install profiler's libraries and public header
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

*A full example of how to compile and run:*

.. code-block:: shell

   mpifort -fc=clang++ example.f90 -I./include -L./lib64 -lprofiler_f -lprofiler_c -lstdc++ -fopenmp
   mpirun -n 6 ./a.out

Including profiler in a CMake project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TBD

API calls
---------

The profiler's API consists of three primary functions; start, stop, and write.
A new timed region is declared by a start-stop pair. Internally, these regions
are defined by hashing the region name it is given at start time. 

C++
^^^

.. code-block:: cpp

   // Prerequisites 
   #include "profiler.h"

   // Start
   size_t profiler_hash = prof.start("Main region");

   // Stop
   prof.stop(profiler_hash);

   // Write
   prof.write();

Fortran
^^^^^^^

.. code-block:: f90

   ! Prerequisites
   use profiler_mod
   integer (kind=pik) :: profiler_hash

   ! Start
   call profiler_start(profiler_hash, "Main region")

   ! Stop
   call profiler_stop(profiler_hash)

   ! Write
   call profiler_write()

Dos and don'ts
^^^^^^^^^^^^^^

Do:

* Initialise MPI before profiling
* Nest timed regions nicely (no overlap)

Don't:

* Add milk to your bowl before cereal

Examples
^^^^^^^^

More fleshed out examples can be found in the ``profiler/tests/system_tests`` directory.

Interpreting output
-------------------

By default profiler will write information out into one file per MPI rank.
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

     * drhook  - Mimics the output format of the DrHook profiling tool so that
                 the same post-processing techniques can be used.
     * threads - Threads have their own seperate table of walltimes.


Examples
^^^^^^^^

TODO
