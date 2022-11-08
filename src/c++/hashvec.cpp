/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <algorithm>
#include <iomanip>
#include <iostream>

#include "hashvec.h"

std::vector<std::pair<size_t,HashEntry>>* HashVec::get()
{
  return hashvec_;
}

void HashVec::sort()
{
    // Sort from high to low self_walltime_
    std::sort
    (
        begin(hashvec_), end(hashvec_),
        [] (auto a, auto b) { 
            return a.second.self_walltime_ > b.second.self_walltime_; 
        }
    );
}

void HashVec::write() const
{

  // Headers
  std::string column = "     ";

  std::cout << "\n";
  std::cout
    << std::setw(3) << std::left  << "#"     
    << std::setw(8) << std::left  << "% Time"     << column
    << std::setw(8) << std::right << "Cumul"      << column
    << std::setw(8) << std::right << "Self"       << column
    << std::setw(8) << std::right << "Total"      << column
    << std::setw(5) << std::right << "calls"      << column
    << std::setw(8) << std::right << "Self"       << column
    << std::setw(8) << std::right << "Total"      << column
    << std::setw(8) << std::right << "Routine@"   << "\n";
  std::cout 
    << std::setw(75) << " " 
    << std::setw(45) << std::left << "(Size; Size/sec; Size/call; MinSize; MaxSize)" << "\n";
  
  // Subheaders
  std::cout 
    << std::setw(3) << std::left  << ""     
    << std::setw(8) << std::left  << "(self)"     << column
    << std::setw(8) << std::right << "(sec)"      << column
    << std::setw(8) << std::right << "(sec)"      << column
    << std::setw(8) << std::right << "(sec)"      << column
    << std::setw(5) << std::right << ""           << column
    << std::setw(8) << std::right << "ms/call"    << column
    << std::setw(8) << std::right << "ms/call"    << column
    << std::setw(8) << std::right << ""           << "\n\n";

  // Find the highest walltime in table_, which should be the total runtime of
  // the program. This is used later when calculating '% Time'.
  double top_walltime = std::max_element
  ( 
    std::begin(hashvec_), std::end(hashvec_),
    [] (auto a, auto b) {
      return a.second.total_walltime_ < b.second.total_walltime_; 
    } 
  )->second.total_walltime_.count(); 

  // Declare any variables external to HashEntry
  int             region_number = 0;
  double          percent_time;
  time_duration_t cumul_walltime = time_duration_t::zero();
  double          self_per_call;
  double          total_per_call;
  
  // 
  // Write data to file
  // 

  std::cout << std::fixed << std::showpoint << std::setprecision(3);

  for (auto& [hash, entry] : hashvec_) {

    // Calculate non-HashEntry data
    region_number++;
    percent_time    = 100.0 * ( entry.self_walltime_.count() / top_walltime );
    cumul_walltime += entry.self_walltime_;
    self_per_call   = 1000.0 * ( entry.self_walltime_.count()  / static_cast<double>(entry.call_count_) );
    total_per_call  = 1000.0 * ( entry.total_walltime_.count() / static_cast<double>(entry.call_count_) );

    // Write everything out 
    std::cout
      << std::setw(3) << std::left  << region_number    
      << std::setw(8) << std::left  << percent_time                  << column
      << std::setw(8) << std::right << cumul_walltime.count()        << column
      << std::setw(8) << std::right << entry.self_walltime_.count()  << column
      << std::setw(8) << std::right << entry.total_walltime_.count() << column
      << std::setw(5) << std::right << entry.call_count_             << column
      << std::setw(8) << std::right << self_per_call                 << column
      << std::setw(8) << std::right << total_per_call                << column
      << std::setw(8) << std::right << entry.region_name_            << "\n";

  }

}