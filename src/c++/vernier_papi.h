/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2024 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   vernier_papi.h
 *  @brief  Declare papi functionalities.
 *
 *  Contains abstractions for functions and classes that manage PAPI.
 *
 */



#ifndef VERNIER_PAPI_H
#define VERNIER_PAPI_H


namespace meto {

  // Functions prototypes
  void papi_init(int);
  void papi_finalize();

} // namespace meto

#endif
