.. -----------------------------------------------------------------------------
    (c) Crown copyright 2026 Met Office. All rights reserved.
    The file LICENCE, distributed with this code, contains details of the terms
    under which the code may be used.
   -----------------------------------------------------------------------------

Post Processing Design
======================

The post processing is delivered in Python, independent of the code calipering functionality.

The post processing is implemented as a library, which can be used by Python programmes, and as a set of callable scripts.

Dependency Management
---------------------

Vernier post-processing is a lightweight pure python implementation, intended for use on a range of platforms,
some of which may have highly customised environments and setups.

For this reason, Vernier post-processing has a dependency constraint that it shall only depending on imports available from
the Python standard library.

‎This is to avoid the dependency management challenges across platforms of complicated but useful modules such as numpy.

Environments which do have rich support can always use Vernier as part of a richer anaysis environment,
but dependency imports beyond the standard library shall not be allowed.

Callable Script Design Constraints
----------------------------------

Callable scripts enable the outputs from Verniered runs to be parsed without writing code.

For maintainability and ease of use reasons, these are limited by design constraints:

* Inputs only: locations (file path) 
* Outputs only: text to stdout only 

Python Module Design Constraints
--------------------------------

The importable Python modules enable programmatic interaction with data parsed from Vernier output files.

For maintainability reasons, these are limited by design constraints:

* Enable parsing of Vernier structured data files.
* Enable organsiation and aggregation of Vernier data.
* No plotting, ever (this is user responsibility).
* Functionality shall be tested.
