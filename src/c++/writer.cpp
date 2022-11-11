/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include "formatter.h"
#include <mpi.h>
#include <fstream>

/**
 * @brief Writer constructor.
 *
 */

Writer::Writer(std::function<void(std::function<void(std::ofstream&, Formatter&)>, std::vector<HashTable>)> in) 
    : strategy_(std::move(in)) 
    {} 

/**
 * @brief Method that executes whatever function is passed to strategy_ via the
 *        constructor.
 *
 */

void Writer::executeStrategy(std::function<void(std::ofstream&, Formatter&)> f_in, std::vector<HashTable> htv)
{
    strategy_(f_in, htv);
}

/**
 * @brief The multiple-output-files strategy.
 * 
 * Writes out data from the vector of HashTables given to it into one file per
 * mpi rank.
 *
 */

void Writer::MultiFile(std::function<void(std::ofstream&, Formatter&)> f_in, std::vector<HashTable> htv)
{
    // Find current MPI rank
    int current_rank;
    MPI_Comm prof_comm_ = MPI_COMM_WORLD;
    MPI_Comm_rank(prof_comm_, &current_rank);

    // For later appending onto the end of the output file name 
    std::string mpi_filename_tail = "-" + std::to_string(current_rank);

    // Pickup environment variable filename if it exists, if not use the 
    // default name of "profiler-output". In either case, include the MPI rank
    // in the name of the file.
    std::string out_filename;
    const char* env_variable = std::getenv("PROF_OUTFILE");
    if (env_variable != NULL)
    {
        out_filename = static_cast<std::string>(env_variable) + mpi_filename_tail;
    }
    else
    {
        out_filename = "profiler-output" + mpi_filename_tail;
    }
    std::ofstream output_stream;
    output_stream.open(out_filename);

    //
    // Write to file 
    //
    Formatter formatter(f_in);

    for (auto& it : htv)
    {
        it.append_to(formatter.get_hashvec());
    }

    formatter.sort_hashvec();
    formatter.executeStrategy(output_stream, formatter);
    
    output_stream.flush();
    output_stream.close();
}