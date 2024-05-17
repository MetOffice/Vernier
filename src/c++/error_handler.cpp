/* -----------------------------------------------------------------------------
*  (c) Crown copyright 2024 Met Office. All rights reserved.
*  The file LICENCE, distributed with this code, contains details of the terms
*  under which the code may be used.
* -----------------------------------------------------------------------------
*/

#include <string>
#include <iostream>

#include "error_handler.h"
#include "vernier_mpi.h"

/**
 * @brief Error handler class
 *
 * @param [in] customError The error as a string
 * @param [in] errorCode The code for the error
 */

meto::error_handler::error_handler (const std::string &customError, int errorCode)
{
    MPI_Comm comm = MPI_COMM_WORLD;
    std::cerr << customError << "\n";
    MPI_Abort(comm, errorCode);
}
