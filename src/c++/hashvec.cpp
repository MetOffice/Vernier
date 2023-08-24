/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashvec.h"

/**
 * @brief  Constructs a new region record.
 * @param [in]  region_hash  Hash of the region name.
 * @param [in]  region_name  The region name.
 * @param [in]  tid          The thread id.
 */

meto::RegionRecord::RegionRecord(size_t const region_hash,
                                 std::string_view const region_name,
                                 int tid)
: region_hash_(region_hash)
, region_name_(region_name)
, total_walltime_      (time_duration_t::zero())
, total_raw_walltime_  (time_duration_t::zero())
, self_walltime_       (time_duration_t::zero())
, child_walltime_      (time_duration_t::zero())
, overhead_walltime_   (time_duration_t::zero())
, call_count_(0)
{
  decorated_region_name_ = region_name_;
  decorated_region_name_ += '@';
  decorated_region_name_ += std::to_string(tid);
}

