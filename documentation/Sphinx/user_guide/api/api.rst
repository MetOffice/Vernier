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

The C++ interface is implemented through the ``Vernier`` class in the
``meto`` namespace.  In the event of an error, the Vernier library
will force the application to exit. A static instance of the Vernier class  called
``vernier`` is instantiated in the ``vernier.h`` file. The member functions are as
follows:

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

The library can be linked to an application with the ``-lvernier`` flag.

CMake Support
"""""""""""""

Vernier includes support for building applications using CMake.  In
order to use this, it is necessary to ensure the directory where Vernier is installed has been
added to your ``$CMAKE_PREFIX_PATH`` environment variable.

Fortran
^^^^^^^
The Fortran interface is a reduced set of the full functionality of the C++ library. The interface
subroutines contained in the ``vernier_mod`` Fortran module are:

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

The library can be linked to an application with the ``-lvernier
-lvernier_c -lvernier_f`` flags.

Guidelines For Use
^^^^^^^^^^^^^^^^^^

To minimise the chances of an error when using Vernier, adhere to the
following guidelines:

**Do**:

* Initialise MPI before profiling.
* Nest timed regions nicely

**Do not**:

* Use a singular hash (or "handle") for all regions.
* Have a stop calliper after any ``return`` statements
* Overlap timed regions
* Exceed the maximum label length

Examples
^^^^^^^^
.. TODO: Update the names of the Profiler class and "prof" object, and update
         the instructions accordingly.

The following simple examples show how to add Vernier API calls to C++
and Fortran programs.

Note that these examples assume that the Vernier libraries have been
installed in a directory called ``lib64``.  If you are installing on a
system based on Debian or a system that is not 64 bit, the libraries
will instead be installed in a directory called ``lib``.

C++
"""

The following shows how to add Vernier calls to an MPI C++ program:

.. code-block:: cpp

   #include "mpi.h"
   #include "vernier.h"

   int main(int argc, char *argv[])
   {
     MPI_Init(&argc, &argv);

     meto::vernier.init();

     // Start measuring a region
     auto vernier_handle = meto::vernier.start("Main region");

     // Work functions go here

     // Stop measuring
     meto::vernier.stop(vernier_handle);

     // Write
     meto::vernier.write();

     // Finalize Vernier
     meto::vernier.finalize();

     MPI_Finalize();
     return 0;
   }

This can be compiled as follows, where `$VERNIER` is an environment
variable that points to the directory where the library has been
installed:

.. code-block:: shell

  mpic++ example.cpp -I$VERNIER/include -L$VERNIER/lib64 \
         -Wl,-rpath=$VERNIER/lib64 -lvernier

The following shows an example of a C++ program which uses Vernier but which
does not make use of MPI:

.. code-block:: cpp

   #include "vernier.h"

   int main()
   {
     meto::vernier.init();

     // Start measuring a region
     auto vernier_handle = meto::vernier.start("Main region");

     // Work functions go here

     // Stop measuring
     meto::vernier.stop(vernier_handle);

     // Write
     meto::vernier.write();
     
     // Finalize Vernier
     meto::vernier.finalize();

     return 0;
   }

This example can be compiled as follows:

.. code-block:: shell

  c++ example.cpp -I$VERNIER/include -DUSE_VERNIER_MPI_STUB \
      -L$VERNIER/lib64 -Wl,-rpath=$VERNIER/lib64 -lvernier

Fortran
"""""""

The following shows how to add Vernier calls to a Fortran program which
makes use of MPI:

.. code-block:: f90

   program example

     use vernier_mod

     integer :: ierror
     integer (kind=vik) :: vernier_handle

     call MPI_Init(ierror)

     ! Initialise
     call vernier_init(MPI_COMM_WORLD)

     ! Start
     call vernier_start(vernier_handle, "Main region")

     ! Work functions go here

     ! Stop
     call vernier_stop(vernier_handle)

     ! Write
     call vernier_write()
     
     ! Finalize Vernier
     call vernier_finalize()

     call MPI_Finalize(ierror)

   end program example

This can be compiled as follows:

.. code-block:: shell

   mpif90 example.f90  -I$VERNIER/include -L$VERNIER/lib64 \
          -Wl,-rpath=$VERNIER/lib64 \
          -lvernier -lvernier_c -lvernier_f

CMake
"""""

The following example of a ``CMakeLists.txt`` file shows how to use
some standard CMake functions to locate the MPI and Vernier packages,
and how to use the results to add the appropriate compiler flags to
a source file:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.10)
   project(myproject VERSION 1.0)

   find_package(MPI REQUIRED)
   find_package(vernier REQUIRED)

   add_executable(example example.cxx)
   target_link_libraries(example PUBLIC MPI::MPI_CXX)
   target_link_libraries(example PUBLIC vernier::vernier)
