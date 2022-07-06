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
#include <cstring>

extern "C" {
	void c_profiler_start(long int&, char const*);
	void c_profiler_stop (long int const&);
	void c_profiler_write();
}

/*
 * @brief Start timing a named region.
 */

void c_profiler_start(long int& hash_out, char const* name )
{
  size_t hash = prof.start( name );

  size_t const hash_size = sizeof(hash);

  // Ensure that the source and destination have the same size.
  static_assert(sizeof(hash) == sizeof(hash_out), "Hash/Out size mismatch.");
  std::memcpy(&hash_out, &hash, hash_size);

}

/*
 * @brief Stop timing the region with the specified handle.
 */

void c_profiler_stop( long int const& hash_in  )
{
    size_t hash;

    size_t const hash_size = sizeof(hash);

    // Ensure that the source and destination have the same size.
    static_assert(sizeof(hash) == sizeof(hash_in), "Hash/In size mismatch.");
    std::memcpy(&hash, &hash_in, hash_size);

    prof.stop( hash );
}

/*
 * @brief Write the profile itself.
 */

void c_profiler_write()
{
    prof.write();
}

