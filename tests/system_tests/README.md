# System tests

This directory contains one C++ and one Fortran system test. The idea behind these is to regularly test the installation of our public header files and libraries. Both are included in our CI testing, but for compiling and running these manually see the "Running Manually" section below.

Both tests also highlight how calls to the profiler's API are made in their respective language. 

## Running Manually

After checking out the profiler's repository and navigating to the root directory, an installation can be done by doing the following:

```
mkdir build
cd build
cmake ..
make
make install
```
By default the `include` and `lib` directories will appear in your build directory but this can be changed via the `-DCMAKE_INSTALL_PREFIX` cmake option.

It's now possible to compile and run one of the two system tests, the steps for doing so are outlined below depending on whether you want to run `system_test.cpp` or `system_test.f90`. 

> Note: both examples assume you're still in the build directory. The paths to the system tests and the "include"/"lib" directories ought to be altered if necessary.

### C++

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
mpicxx -o test ../tests/system_tests/system_test.cpp -I./include -L./lib -lprofiler -fopenmp
mpirun -n 4 ./test
```

### Fortran

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
mpifort -o test ../tests/system_tests/system_test.cpp -I./include -L./lib -lprofiler_f -lprofiler_c -fopenmp
mpirun -n 4 ./test
```