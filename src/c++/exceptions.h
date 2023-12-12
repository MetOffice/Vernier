/* -----------------------------------------------------------------------------
*  (c) Crown copyright 2021 Met Office. All rights reserved.
*  The file LICENCE, distributed with this code, contains details of the terms
*  under which the code may be used.
* -----------------------------------------------------------------------------
*/

#include <iostream>
#include <stdexcept>
#include <string>

/**
 * @file   exceptions().h
 * @brief  
 *
 * stuff
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

namespace meto
{
    //Class prototypes
    class exception : public std::exception
    {
    private:
    std::string error = "";

    public:
    explicit exception (std::string customException);
    std::string what ();
    };  
} // End meto namespace
#endif
