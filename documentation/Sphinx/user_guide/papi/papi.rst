.. -----------------------------------------------------------------------------
     (c) Crown copyright 2024 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

.. _papi:

PAPI
----

`PAPI <https://icl.utk.edu/papi/>`__ (Performance Application Programming Interface)
is a library that provides access to hardware performance counters, such as floating-point
operation counts, cache misses, and instruction counts.

When Vernier is built with PAPI support (``-DENABLE_PAPI=ON``, see the
:ref:`build guide <requirements>`), it automatically collects the requested hardware
metrics between every ``start``/``stop`` calliper pair, per thread.

Selecting Events
^^^^^^^^^^^^^^^^

Events are specified via the :ref:`VERNIER_PAPI_EVENTS <env-variables>`
environment variable as a comma-separated list of PAPI event names:

.. code-block:: shell

   export VERNIER_PAPI_EVENTS=PAPI_FP_OPS,PAPI_TOT_INS

If the variable is not set, no hardware metrics are collected and there is minimal
performance impact.

To discover which events are available on the current hardware:

.. code-block:: shell

   papi_avail          # preset events (e.g. PAPI_FP_OPS, PAPI_TOT_INS)
   papi_native_avail   # native hardware events

Capabilities and Limitations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* A maximum of **5 events** can be collected simultaneously. This can
  be changed in the source code by changing the
  ``VERNIER_MAX_PAPI_METRICS`` macro. However, this is still limited
  by the underlying hardware.
* All selected events must come from the **same hardware source**. Mixing events
  from different sources (e.g. CPU counters with network or energy counters) may
  not be supported depending on the underlying hardware.
* PAPI metric collection works with OpenMP. Two patterns are supported:

  * **Inside a parallel region**: each thread has its own ``start``/``stop`` pair
    and accumulates metrics independently.
  * **Outside a parallel region**: a serial region spans an entire OpenMP parallel
    section, with no nested callipers inside that parallel region.

* PAPI metric collection was not tested on the following:

  * Nested OpenMP parallelism.
  * Other non-OpenMP threading system.
  * Without OpenMP

* The DRHOOK output format do not print the PAPI metrics.

Output
^^^^^^

When PAPI events are active, the ``threads`` output format appends one column per
event to each row. The column header is the (truncated if needed) event name.

**Example with** ``VERNIER_PAPI_EVENTS=PAPI_FP_OPS,PAPI_TOT_CYC``:

.. code-block:: text

    Region                                         Self (s)      Total (s)   Overhead (s)     Calls    PAPI_FP_OPS   PAPI_TOT_CYC
     --------------------------------------- -------------- -------------- -------------- --------- -------------- --------------
    dgemm@3                                         19.8223        19.8223              0         1   186968657922    40655117137
    dgemm@2                                         19.8223        19.8223              0         1   186968657922    40653663813
    dgemm@1                                         19.8223        19.8223              0         1   186968657922    40653761713
    dgemm@0                                         19.8223        19.8223              0         1   186968657922    40653539466
    dot@0                                          0.234555       0.234555              0         1       83886242      734477645
    dot@2                                          0.234555       0.234555              0         1       83886242      734516792
    dot@3                                          0.234554       0.234554              0         1       83886242      734688082
    dot@1                                          0.234554       0.234554              0         1       83886242      734734336
    triad@2                                       0.0519043      0.0519043              0         1        8388610      162768033
    triad@3                                       0.0519034      0.0519034              0         1        8388610      162760542
    triad@0                                       0.0519033      0.0519033              0         1        8388610      162738698
    triad@1                                       0.0519029      0.0519029              0         1        8388610      162716405
    daxpy@0                                       0.0418477      0.0418477              0         1        8388611      131025661
    daxpy@1                                       0.0417583      0.0417583              0         1        8388611      130775445
    daxpy@3                                        0.041732       0.041732              0         1        8388611      130800846
    daxpy@2                                       0.0417312      0.0417312              0         1        8388611      130693844
    __vernier__@3                                0.00033024     0.00033024              0         4              0              0
    __vernier__@2                               0.000329868    0.000329868              0         4              0              0
    __vernier__@1                               0.000305467    0.000305467              0         4              0              0
    __vernier__@0                               0.000243679    0.000243679              0         5              0              0
    main_init@0                                  9.0485e-05     9.0485e-05              0         1              4         848516
