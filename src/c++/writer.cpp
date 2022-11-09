/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include <mpi.h>
#include <fstream>

/**
 * @brief Writer constructor.
 *
 */

Writer::Writer(std::function<void(std::vector<HashTable>)> in) 
    : strategy_(std::move(in)) 
    {} 

/**
 * @brief Method that executes whatever function is passed to strategy_ via the
 *        constructor.
 *
 */

void Writer::executeStrategy(std::vector<HashTable> htv)
{
    strategy_(htv);
}

/**
 * @brief The multiple-output-files strategy.
 * 
 * Writes out data from the vector of HashTables given to it into one file per
 * mpi rank.
 *
 */

void Writer::MultiFile(std::vector<HashTable> htv)
{
    // Find current MPI rank
    int current_rank;
    MPI_Comm prof_comm_ = MPI_COMM_WORLD;
    MPI_Comm_rank(prof_comm_, &current_rank);

    // Filename "tail" - will be different for each rank and appended onto the
    // end of the output file name 
    std::string mpi_filename_tail = "-" + std::to_string(current_rank);

    // Pickup environment variable filename if it exists, if not use the 
    // default name of "profiler-output". In either case, include the MPI rank
    // in the name of the file.
    const char* env_variable = std::getenv("PROF_OUTFILE");
    std::string out_filename;
    if (env_variable != NULL)
    {
        out_filename = env_variable + mpi_filename_tail;
    }
    else
    {
        delete env_variable;
        out_filename = "profiler-output" + mpi_filename_tail;
    }
    std::ofstream output_stream;
    output_stream.open(out_filename);

    //
    // Write to file 
    //
    for (auto& it : htv)
    {
        it.print(output_stream);
    }

    output_stream.flush();
    output_stream.close();
}