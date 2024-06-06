.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

Interpreting Output
-------------------

By default Vernier will write information out into one file per MPI rank.
Single file output is not currently supported but is achievable via
post-processing techniques.

Each file contains a table of the all the declared regions and their
associated walltimes.

The :ref:`environment variables <env-variables>` section at the end of the
setting up guide outlines environment variables relevant to Vernier's output.
To reiterate, the two output format options are **threads** and **drhook**.

**Example "threads" output:**

.. code-block:: text

    Thread: All                                     Self (s) Total (raw) (s)       Total (s)      Calls
    ---------------------------------------- --------------- --------------- --------------- ----------
    LAPACK_zheev@0                                  0.176658        0.176658        0.176658       3141
    PRINT_EIGENVALUES@0                             0.109331        0.109331        0.109331       3141
    MAIN@0                                         0.0174985        0.320754        0.323922          1
    Z_HAMILTONIAN_2ND@0                           0.00797964      0.00797964      0.00797964       3141
    Z_HAMILTONIAN_INIT@0                          0.00691802      0.00691802      0.00691802       3141
    __profiler__@0                                0.00316826      0.00316826      0.00316826      15706
    Z_HAMILTONIAN_THIRD@0                         0.00236861      0.00236861      0.00236861       3141

* Self: Total time spent inside the corresponding region only (excluding calls
  to other regions).
* Total: Total time spent between the two callipers for this region including
  calls to elsewhere.
* The "Total (raw)" column is the total time spent in a code region when the
  overhead from Vernier is taken into account. In the example above this
  overhead is only noticable with the overarching main program.

**Example "drhook" output:**

.. code-block:: text

    Profiling on 4 thread(s).

    #  % Time         Cumul         Self        Total     # of calls        Self       Total    Routine@
                                                                             (Size; Size/sec; Size/call; MinSize; MaxSize)
        (self)        (sec)        (sec)        (sec)                    ms/call     ms/call

    1   54.756        0.178        0.178        0.178           3141       0.057       0.057    LAPACK_zheev@0
    2   34.040        0.288        0.110        0.110           3141       0.035       0.035    PRINT_EIGENVALUES@0
    3    4.782        0.304        0.016        0.324              1      15.515     324.418    MAIN@0
    4    2.500        0.312        0.008        0.008           3141       0.003       0.003    Z_HAMILTONIAN_2ND@0
    5    2.141        0.319        0.007        0.007           3141       0.002       0.002    Z_HAMILTONIAN_INIT@0
    6    1.033        0.322        0.003        0.003          15706       0.000       0.000    __profiler__@0
    7    0.748        0.324        0.002        0.002           3141       0.001       0.001    Z_HAMILTONIAN_THIRD@0

* % Time: The percentage of the overall time this region took up (calculated
  from self time).

  .. warning::

    The percentage is calculated from the highest total walltime of all profiled
    regions, so it is therefore assumed that a top-level pair of callipers
    encompassing the entire program is in place.

* Cumul: The cumulative self time so far (the self times are ordered from high
  to low, this column sums the self times as it descends down the list).
* Self: Total time spent inside the corresponding region only (excluding calls
  to other regions).
* Total: Total time spent between the two callipers for this region including
  calls to elsewhere.
* The self and total time per call (in ms) is also given.

In both examples the ``@0`` appended onto the end of all region names indicates
the OpenMP thread number.