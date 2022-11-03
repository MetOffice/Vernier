/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "writer.h"
#include <mpi.h>

/**
 * @brief  Writes out all entires in the hashtable into a separate file for 
 *         each mpi rank.
 * 
 * @param[in] htvec  A vector of hashtables.
 *
 */

void MultipleFiles::write(std::vector<HashTable> htvec) 
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
    output_stream.open(out_filename);

    // Write to file then close it
    for (auto& it : htvec)
    {
        it.print(output_stream);
    }
    output_stream.flush();
    output_stream.close();
}

/**
 * @brief  Context class constructor.
 * 
 * @param[in] temp_ptr  Pointer to IO Interface, which will be empty after 
 *                      ownership is transferred to io_strategy_
 *
 */

IO_StrategyContext::IO_StrategyContext(std::unique_ptr<IO_Interface> temp_ptr) 
    : io_strategy_(std::move(temp_ptr)) 
    {}

/**
 * @brief  Allows the strategy to be switched.
 * 
 * @param[in] temp_ptr  Pointer to IO Interface, which will be empty after 
 *                      ownership is transferred to io_strategy_
 *
 */

void IO_StrategyContext::setStrategy(std::unique_ptr<IO_Interface> temp_ptr)
{
    io_strategy_ = std::move(temp_ptr);
}

/**
 * @brief  Calls the write method linked to whichever derived class the 
 *         strategy is referencing.
 * 
 * @param[in] htvec  A vector of hashtables.
 *
 */

void IO_StrategyContext::executeStrategy(std::vector<HashTable> htvec)
{
    io_strategy_->write(htvec);
}

/**
 * @brief  Strategy decision is made and context class object created in order
 *         to execute the appropriate write method.
 * 
 * @param[in] htvec  A vector of hashtables.
 *
 */

void Writer::write(std::vector<HashTable> htvec)
{
    const char* user_strat = std::getenv("PROF_IO_MODE");
    if ( user_strat == NULL || static_cast<std::string>(user_strat) == "MultipleFiles")
    {
        IO_StrategyContext contextObj(std::move(std::make_unique<MultipleFiles>()));
        contextObj.executeStrategy(htvec);
    }
    else throw std::runtime_error("Invalid PROF_IO_MODE value");
}