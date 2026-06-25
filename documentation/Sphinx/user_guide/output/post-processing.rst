.. -----------------------------------------------------------------------------
     (c) Crown copyright 2025 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

Post-processing
===============

Vernier comes with a small library and a script to help with post-processing of
the output data. To keep things as simple and easy to use as possible, the
library is written in pure Python and has no dependencies on any other
libraries not included in the core Python distribution.

The library is located in the ``vernier/post_processing`` directory and comes
with a pyproject.toml file, so it can be installed into a virtual environment
using pip.

Scripts
-------

.. dropdown:: summarise_vernier

    Processes parallel Vernier output into a single table of results. The script
    takes a single argument, which can be either a directory containing the
    multiple output files from a parallel run, or a single output file from a
    collated run.

