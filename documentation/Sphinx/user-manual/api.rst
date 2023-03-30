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

.. doxygenfunction:: profiler_write

.. doxygenfunction:: profiler_stop

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

Although their primary purpose is for system testing, the tests in
``profiler/tests/system_tests`` also serve as more fleshed-out examples on how
calls to profiler are made. 
