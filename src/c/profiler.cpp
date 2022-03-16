/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
--------------------------------------------------------------------------------
 Description

 Created by Andrew Coughtrie.
\*----------------------------------------------------------------------------*/

#include "profiler.h"

#include <iostream>

extern "C" {

size_t c_profiler_start_( const char *name )
{
    return prof.start( name );
}

void c_profiler_stop_( const size_t hash )
{
    prof.stop( static_cast<size_t>( hash ) );
}

void c_profiler_write_()
{
    prof.write();
}

} // end extern "C"