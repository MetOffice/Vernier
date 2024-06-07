.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

Testing
=======

Tests are in the repository 
`here <https://github.com/MetOffice/Vernier/tree/main/tests>`__.

Unit Tests
----------

The C++ and Fortran unit tests utilise the GoogleTest and pFUnit frameworks, 
respectively. 

Running
^^^^^^^

.. code-block:: shell

   mkdir build
   cd build
   cmake ..
   make 
   # Either 'make test' or 'ctest'.
   make test
