.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

API
---

Vernier's API consists of five primary functions; ``init``, ``start``, ``stop``
``finalise``, and ``write``. These functions are used to manage the profiling.
A timed region is defined by a start-stop pair which return and take a hash
respectively.

How calls to these functions are made differs slightly between C++ and Fortran,
but the core functionality is the same.

There is a global Vernier object that can be used without needing to create an
instance of the Vernier class manually. This object is called ``vernier`` and is
used by the Fortran API.

C++
^^^

.. cpp:class:: meto::Vernier

   .. cpp:function:: void init(MPI_Comm const client_comm_handle = MPI_COMM_WORLD)

       Initialises the Vernier profiler with the specified MPI communicator.
       This function must be called before any calls to ``start``.
       The `comm` parameter is used to specify the MPI communicator for parallel
       execution, the default when unspecified is ``MPI_COMM_WORLD``.

   .. cpp:function:: size_t start(std::string_view const region_name)

       Starts a timed region with the given name. Returns a handle (or "hash") for
       the region.

   .. cpp:function:: void stop(size_t const &hash)

       Stops the timed region associated with the given handle.

   .. cpp:function:: void write()

       Writes the profiling data to the output file.

   .. cpp:function:: void finalise()

       Finalises the Vernier profiler, ensuring all data is written and resources
       are cleaned up. This function should be called at the end of the program.

   .. cpp:function:: double get_total_walltime(size_t const &hash, int const thread_id)

       Returns the total (inclusive) time taken by a region and everything below it

   .. cpp:function:: double get_overhead_walltime(size_t const hash, int const thread_id)

       Returns the profiling overhead time experienced by a region, as incurred by
       calling child regions.

   .. cpp:function:: double get_self_walltime(size_t const hash, int const input_tid)

       Returns the self time of a region, which is the time spent in that region
       excluding any child regions.

   .. cpp:function:: double get_child_walltime(size_t const hash, int const input_tid) const

       Returns the child time of a region, which is the time spent in child regions
       called by that region including their descendants.

   .. cpp:function:: std::string get_decorated_region_name(size_t const hash, int const input_tid) const

       Returns the name of a region corresponding to a given hash.

   .. cpp:function:: unsigned long long int get_call_count(size_t const hash, int const input_tid) const

       Returns the number of times a region has been called on the input thread ID.

   .. cpp:function:: unsigned long long int get_prof_call_count(int const input_tid) const;

       Returns the number of calliper pairs called on the specified thread.


Fortran
^^^^^^^


.. function:: vernier_init(client_comm_handle)

   :param integer: client_comm_handle: MPI communicator (default is ``MPI_COMM_WORLD``)

   Initialises the Vernier profiler with the specified MPI communicator.
   This function must be called before any calls to ``vernier_start``.
   The `client_comm_handle` parameter is used to specify the MPI communicator
   for parallel execution, the default when unspecified is ``MPI_COMM_WORLD``.

.. function:: vernier_start(vernier_handle, region_name)

   :param integer: vernier_handle: Handle for the timed region (hash
   :param string: region_name: Name of the timed region

   Starts a timed region with the given name ``region_name``. Returns a handle (i.e. a hash) for
   the region in ``vernier_handle``.

.. function:: vernier_stop(vernier_handle)

   :param integer: vernier_handle: Handle for the timed region (hash)

   Stops the timed region associated with the given handle.

.. function:: vernier_write()

   Writes the profiling data to the output file.

.. function:: vernier_finalise()

   Finalises the Vernier profiler, ensuring all data is written and resources
   are cleaned up. This function should be called at the end of the program.



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
