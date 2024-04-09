/* -----------------------------------------------------------------------------
*  (c) Crown copyright 2021 Met Office. All rights reserved.
*  The file LICENCE, distributed with this code, contains details of the terms
*  under which the code may be used.
* -----------------------------------------------------------------------------
*/

#include "error_handler.h"
#include <mpi.h>
#include <string>
#include <iostream>

/**
 * @brief Error handler class
 *
 * @returns Exit/aborts the code when an error is encountered and calls MPI_Abort/Finalize to deal with MPI code.
 */

meto::error_handler::error_handler (const std::string &customError, int errorCode)
{
    MPI_Comm comm = MPI_COMM_WORLD;
    
    std::cerr << customError << "\n";
    //exit (errorCode);

    //MPI_Init(NULL, NULL);
    MPI_Abort(comm, errorCode);
    //MPI_Finalize();
}