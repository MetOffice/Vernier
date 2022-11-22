/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include "formatter.h"
#include <mpi.h>

void Multifile::write()
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
    
    output_stream.open(out_filename);
}

void Multifile::VisitStandard(const std::unique_ptr<Standard> standard, std::vector<std::pair<size_t, HashEntry>> hashvec)
{
    write();
    standard->write(output_stream, hashvec);
    output_stream.flush();
    output_stream.close();
}

void Multifile::VisitDrHook(const std::unique_ptr<DrHook> drhook, std::vector<std::pair<size_t, HashEntry>> hashvec)
{
    write();
    drhook->write(output_stream, hashvec);
    output_stream.flush();
    output_stream.close();
}

