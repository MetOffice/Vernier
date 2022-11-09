/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <algorithm>
#include <iomanip>
#include <iostream>

#include "hashvec.h"

/**
 * @brief Getter that returns a reference to the private hashvec.
 * 
 * @param[out]  hashvec_  The hashvec itself, a vector of key-value pairs.
 *
 */

std::vector<std::pair<size_t,HashEntry>>& HashVec::get()
{
  return hashvec_;
}

/**
 * @brief CFunction that sorts all entries from high to low self walltime.
 *
 */

void HashVec::sort()
{
  std::sort
  (
      begin(hashvec_), end(hashvec_),
      [] (auto a, auto b) { 
          return a.second.self_walltime_ > b.second.self_walltime_; 
      }
  );
}

/**
 * @brief Write all data to file.
 *
 */

void HashVec::write() const
{
  // Preliminary info
  std::cout << "        " << "No. of instrumented routines called : 9\n";
  std::cout << "        " << "Instrumentation started : 20100521 171238\n";
  std::cout << "        " << "Instrumentation   ended : 20100521 172033\n";
  std::cout << "        " << "Instrumentation overhead: 0.90%\n";
  std::cout << "        " << "Memory usage : Memory usage : 1346 MBytes (heap), 1315 MBytes (rss), 796 MBytes (stack), 1116 (paging)\n";
  std::cout << "        " << "Wall-time is 472.28 sec on proc#1 (192 procs, 1 threads)\n";
  std::cout << "        " << "Thread#1:      472.28 sec (100.00%)\n" << std::endl;

  // Table Headers
  std::cout 
    << "    "
    << std::setw(3) << std::left   << "#"     
    << std::setw(7) << std::left   << "% Time"     
    << std::setw(13) << std::right << "Cumul"      
    << std::setw(13) << std::right << "Self"       
    << std::setw(13) << std::right << "Total"      
    << std::setw(15) << std::right << "# of calls"
    << std::setw(12) << std::right << "Self"      
    << std::setw(12) << std::right << "Total"      << "    "
                                   << "Routine@"   << "\n";
  std::cout << "    "
    << std::setw(73) << "" 
    << "(Size; Size/sec; Size/call; MinSize; MaxSize)" << "\n";
  
  // Subheaders
  std::cout 
    << "    "
    << std::setw(3)  << std::left  << ""     
    << std::setw(7)  << std::right << "(self)"    
    << std::setw(13) << std::right << "(sec)"     
    << std::setw(13) << std::right << "(sec)"     
    << std::setw(13) << std::right << "(sec)"     
    << std::setw(15) << std::right << ""          
    << std::setw(12) << std::right << "ms/call"   
    << std::setw(12) << std::right << "ms/call"   
                                   << "\n\n";

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
      << "    "
      << std::setw(3)  << std::left  << region_number    
      << std::setw(7)  << std::right << percent_time                 
      << std::setw(13) << std::right << cumul_walltime.count()       
      << std::setw(13) << std::right << entry.self_walltime_.count() 
      << std::setw(13) << std::right << entry.total_walltime_.count()
      << std::setw(15) << std::right << entry.call_count_            
      << std::setw(12) << std::right << self_per_call                
      << std::setw(12) << std::right << total_per_call                << "    "        
                                     << entry.region_name_            << "\n";

  }

}