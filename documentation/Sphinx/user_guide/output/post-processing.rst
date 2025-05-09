.. -----------------------------------------------------------------------------
     (c) Crown copyright 2025 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

Post-processing
---------------

The file 'post-processing.py' can be used for processing the raw outputs 
into a more usable format, merging all MPI ranks to give the means
across all ranks.

Please note that you will need an environment with the pandas library. This is 
included within the Vernier conda environment.

.. dropdown:: ``Creating/ loading conda environments in Bash``
	
  1. Navigate to the .yml file

  .. code-block:: shell

    cd etc/conda/

  2. Create the environment from the .yml file

  .. code-block:: shell

    conda env create -f vernier-env.yml

  3. Activate the environment

  .. code-block:: shell

    conda activate vernier-env

With an appropriate environment loaded, run the script with:

.. code-block:: shell

    ./post-processing/post-process.py -path=path/to/your/vernier/outputs...

There are a number of command line options that can be passed through the
script, these are given in the table below.

..  list-table::
    :widths: 20 15 30
    :header-rows: 1

    * - Argument
      - Default
      - Description
    * - ``-help``
      - N.A
      - Lists all command-line arguments and gives usage
    * - ``-path``
      - Current working directory
      - The path containing the vernier outputs to be processed
    * - ``-outputname``
      - vernier-merged-output
      - Name of file to write to
    * - ``-inputname``
      - vernier-output-
      - Vernier files to read from

Below is an example of the current script output.
This removes a lot of the raw data Vernier collects, most of which
will be added back at a later date.

.. code-block:: text

    Total       Self                            Routine
  534.9714    14.4320    __lfric_atm_timestep_global__
    0.0754     0.0754                      __vernier__
   36.1004     3.8090             calc_phys_predictors
   45.7812    45.7812                     fast_physics
  122.7792    16.0232         gungho_transport_control
   10.6424    10.6424               map_physics_fields
   49.5298    49.5298           mass_matrix_solver_alg
   45.6938    45.6938   mixed_schur_preconditioner_alg
   94.5162    48.7716                     mixed_solver
   30.8218    24.5758                  rhs_alg_default
   14.5336    14.5336 runtime_constants_dycore_damping
    0.0180     0.0180  runtime_constants_dycore_lagged
    0.0362     0.0362      runtime_constants_si_matrix
   99.4924     4.9744         semi_implicit_solver_alg
  520.5374    57.2040       semi_implicit_timestep_alg
   13.7284     5.4234         si_operators_alg_compute
   86.7186    86.7186                     slow_physics
   47.3598     8.0696                tracer_collection
   92.9250    92.9250        transport_ffsl_horizontal
   31.9320     5.7346               transport_moisture

* Total: Mean total time spent between the two callipers for this region including
  calls to elsewhere.
* Self: Mean total time spent inside the corresponding region only (excluding calls
  to other regions).
* Routine: The name of the callipered region.

