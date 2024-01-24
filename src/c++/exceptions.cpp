/* -----------------------------------------------------------------------------
*  (c) Crown copyright 2021 Met Office. All rights reserved.
*  The file LICENCE, distributed with this code, contains details of the terms
*  under which the code may be used.
* -----------------------------------------------------------------------------
*/

#include "exceptions.h"
#include <mpi.h>
#include <string>
#include <iostream>

/**
 * @brief Exceptions class that extends std::exceptions.
 *
 * @returns An error.
 */

meto::exception::exception (const std::string &customException)
{
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Init(NULL, NULL);
    MPI_Abort(comm, 0);
    MPI_Finalize();
    this->error = customException;
}

const char *meto::exception::what () 
{
    return (this->error).c_str();
}
