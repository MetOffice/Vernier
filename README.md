# profiler
Profiler for scientific code on HPC platforms.

## Building
Requires a c++17 compatible compiler, currently only tested with GCC-9.2.0.

The testing framework is GoogleTest (1.11.0) and source code documentation is created using Doxygen (1.8.5).

**To build:**  
In project root directory.
~~~~~~~~~~~~~~~~shell
mkdir build
cd build
cmake ..
make
make test
~~~~~~~~~~~~~~~~
