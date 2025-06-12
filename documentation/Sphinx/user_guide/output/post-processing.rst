.. -----------------------------------------------------------------------------
     (c) Crown copyright 2025 Met Office. All rights reserved.
     The file LICENCE, distributed with this code, contains details of the terms
     under which the code may be used.
   -----------------------------------------------------------------------------

Post-processing
---------------

The file 'post-processing.py' can be used for processing the raw Vernier outputs 
into a more usable format, merging all MPI ranks to give the means, minimum and
maximum of every callipered region across all ranks.


Please note that you will need an environment with the pandas library. This is 
included within the Vernier conda environment.

.. dropdown:: ``Creating/ loading conda environments in Bash``

  .. code-block:: shell

    conda env create -f ./etc/conda/vernier-env.yml
    conda activate vernier-env

With an appropriate environment loaded, run the script with:

.. code-block:: shell

    ./post-processing/post-process.py --path=path/to/your/vernier/outputs...

There are a number of command line options that can be passed through the
script, these are given in the table below.

..  list-table::
    :widths: 20 15 30
    :header-rows: 1

    * - Argument
      - Default
      - Description
    * - ``-h/ --help``
      - N.A
      - Lists all command-line arguments and gives usage
    * - ``-p/ --path``
      - Current working directory
      - The path containing the vernier outputs to be processed
    * - ``-o/ --output_name``
      - vernier-merged-output
      - Name of file to write to
    * - ``-i/ --input_name``
      - vernier-output-
      - Vernier files to read from
    * - ``-m/ --max_only``
      - False
      - If used will only calculate maximum values across MPI ranks
    * - ``-f/ --full_info``
      - False
      - Enables full Vernier output to be merged and output

Below is an example of the current script output without additional flags.

.. code-block:: text

                           Routine  Mean_Total  Min_Total  Max_Total  Mean_Self   Min_Self   Max_Self
     __lfric_atm_timestep_global__    534.9714    534.958    534.984    14.4320     12.398     15.982
                       __vernier__      0.0754      0.070      0.079     0.0754      0.070      0.079
              calc_phys_predictors     36.1004     35.741     36.468     3.8090      3.690      3.928
                      fast_physics     45.7812     43.848     49.337    45.7812     43.848     49.337
          gungho_transport_control    122.7792    122.530    123.112    16.0232     15.635     16.472
                map_physics_fields     10.6424     10.253     11.045    10.6424     10.253     11.045
            mass_matrix_solver_alg     49.5298     49.218     49.788    49.5298     49.218     49.788
    mixed_schur_preconditioner_alg     45.6938     43.518     47.355    45.6938     43.518     47.355
                      mixed_solver     94.5162     92.243     96.155    48.7716     48.673     48.834
                   rhs_alg_default     30.8218     29.422     31.873    24.5758     23.280     25.547
  runtime_constants_dycore_damping     14.5336     14.059     14.837    14.5336     14.059     14.837
   runtime_constants_dycore_lagged      0.0180      0.016      0.025     0.0180      0.016      0.025
       runtime_constants_si_matrix      0.0362      0.035      0.038     0.0362      0.035      0.038
          semi_implicit_solver_alg     99.4924     97.715    101.074     4.9744      3.963      5.606
        semi_implicit_timestep_alg    520.5374    518.991    522.559    57.2040     52.699     59.931
          si_operators_alg_compute     13.7284     13.713     13.740     5.4234      5.259      5.701
                      slow_physics     86.7186     86.639     86.803    86.7186     86.639     86.803
                 tracer_collection     47.3598     47.205     47.561     8.0696      8.009      8.138
         transport_ffsl_horizontal     92.9250     92.104     93.618    92.9250     92.104     93.618
                transport_moisture     31.9320     31.537     32.329     5.7346      5.712      5.786

* Routine: The name of the callipered region.
* Mean_Total: Mean total time spent between the two callipers for this region including calls to elsewhere.
* Min_Total: Minimum value recorded for the Total value across all ranks
* Max_Total: Maximum value recorded for the Total value across all ranks
* Mean_Self: Mean total time spent inside the corresponding region only (excluding calls to other regions).


