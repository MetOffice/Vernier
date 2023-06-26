Testing
=======

.. TODO: link to where tests live in the repo ?

Unit Tests
----------

The C++ and Fortran unit tests utilise the GoogleTest and pFUnit frameworks 
respectively. 

Running
^^^^^^^

.. code-block:: shell

   mkdir build
   cd build
   cmake ..
   make 
   # either 'make test' or 'ctest'
   make test

Coverage
^^^^^^^^

The unit test code coverage is generated using `gcovr <https://gcvor.com/en/stable/>`_,
which wraps the GCC tool "gcov" with additional functionality. A detailed 
coverage report can be found `here <https://metoffice.github.io/vernier/coverage>`_.

System Tests
------------

The ``Vernier/tests/system_tests`` directory contains one C++ and one Fortran
system test. The idea behind these is to regularly test the installation of the
public header files and libraries.

Both tests are also good examples of how calls to Vernier's API are made
in their respective language.

Running
^^^^^^^

The requirements and supported compilers can be found in the
:ref:`build guide <requirements>`.

Instructions on how to install Vernier's libraries and public header
files can be found :ref:`here <installation>`.

The steps for compiling and running are outlined below depending on whether you
want to run ``system_test.cpp`` or ``system_test.f90``.

.. note::
    
   Both examples assume you're currently in the build directory - if not, the paths to
   the libraries, headers, and system tests themselves ought to be altered accordingly.

C++:

.. code-block:: shell

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
    mpicxx -o test ../tests/system_tests/system_test.cpp -I./include -L./lib -lvernier -fopenmp
    mpirun -n 4 ./test

Fortran:

.. code-block:: shell

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
    mpifort -o test ../tests/system_tests/system_test.cpp -I./include -L./lib -lvernier_f -lvernier_c -fopenmp
    mpirun -n 4 ./test
