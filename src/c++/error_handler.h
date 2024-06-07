/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2024 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include <iostream>
#include <stdexcept>
#include <string>

/**
 * @file   error_handler().h
 * @brief  Header file for error_handler.cpp
 *
 *
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

namespace meto {
// Class prototypes
class error_handler {

public:
  explicit error_handler(const std::string &customError = "",
                         int errorCode = 0);
};
} // namespace meto
#endif
