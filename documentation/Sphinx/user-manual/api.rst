API
---

Vernier's API consists of three primary functions; **start**, **stop**,
and **write**. A timed region is defined by a start-stop pair.

How calls to these functions are made differs slightly between C++ and Fortran,
but the core functionality is the same. 

C++
^^^

.. doxygenclass:: vernier
   :members: start, stop, write

Fortran
^^^^^^^
.. Note: The following function is currently defined manually to avoid errors
         caused by Breathe expecting C++ syntax.
.. cpp:function:: subroutine vernier_mod::vernier_start::vernier_start(hash_out, region_name)

.. doxygenfunction:: vernier_stop

.. doxygenfunction:: vernier_write

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
.. TODO: Update the names of the Profiler class and "prof" object, and update
         the instructions accordingly.

**C++**:

.. code-block:: cpp
 
   #include "vernier.h"

   // Start
   auto vernier_handle = vernier.start("Main region");

   // Stop
   vernier.stop(vernier_handle);

   // Write
   vernier.write();

**Fortran**:

.. code-block:: f90

   use vernier_mod
   integer (kind=pik) :: vernier_handle

   ! Start
   call vernier_start(vernier_handle, "Main region")

   ! Stop
   call vernier_stop(vernier_handle)

   ! Write
   call vernier_write()

Although their primary purpose is for system testing, the tests in
``Vernier/tests/system_tests`` also serve as more fleshed-out examples on how
calls to Vernier are made. 