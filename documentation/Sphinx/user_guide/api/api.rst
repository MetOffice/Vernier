.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

API
---

Vernier's API consists of four primary functions; **init**, **start**,
**stop**, and **write**. A timed region is defined by a start-stop
pair.  The interface to these functions differs between C++ and
Fortran, but the core functionality is the same.

C++
^^^

The C++ interface is implemented through the ``Vernier`` class in the
``meto`` namespace.  In the event of an error, the Vernier library
will force the application to exit.  The member functions are as
follows:

* **init**: initialises the Vernier instance.  Accepts an optional MPI
  communicator.  Defaults to ``MPI_COMM_WORLD`` if not specified.
* **start**: starts a timed region.  Argument is a string representing
  the name of the region
* **stop**: ends an existing timed region.  Argument is a handle which
  represents the start of the region
* **write**: writes the profile information to a file

The library can be linked to an application with the ``-lvernier`` flag.

CMake Support
"""""""""""""

Vernier includes support for building applications using CMake.  In
order to use this, it is necessary to directory where Vernier has been
installed has added to your ``$CMAKE_PREFIX_PATH`` environment
variable.

Fortran
^^^^^^^
.. Note: The following function is currently defined manually to avoid errors
         caused by Breathe expecting C++ syntax.
.. .. cpp:function:: subroutine vernier_mod::vernier_start::vernier_start(hash_out, region_name)

The Fortran interface is implemented through four subroutines which
provide the same functionality as the C++ library.  The interface
subroutines contained in the ``vernier_mod`` Fortran module are:

* **vernier_init**: initialises the Vernier interface.  Accepts an
  optional MPI communicator.  Defaults to ``MPI_COMM_WORLD`` if not
  specified.
* **vernier_start**: starts a timed region.  Arguments are an integer
  of ``kind=vik`` and a a string representing the name of the region
* **vernier_stop**: ends an existing timed region.  Argument is an
  integer of ``kind=vik`` which represents the start of the region
* **vernier_write**: writes the profile information to a file

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

   int
   main(int argc, char *argv[])
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

   int
   main()
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
