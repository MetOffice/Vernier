.. -----------------------------------------------------------------------------
    (c) Crown copyright 2026 Met Office. All rights reserved.
    The file LICENCE, distributed with this code, contains details of the terms
    under which the code may be used.
   -----------------------------------------------------------------------------

Post Processing Design
======================

The post processing is delivered in Python, independent of the code calipering functionality.

The post processing is implemented as a library, which can be used by Python programmes, and as a set of callable scripts.

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
