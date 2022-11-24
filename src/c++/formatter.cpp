/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

#include "formatter.h"
#include <iomanip>
#include <algorithm>

Formatter::Formatter(std::function<void(std::ofstream&, std::vector<std::pair<size_t, HashEntry>>)> format)
  : format_(std::move(format))
  {}

void Formatter::executeFormat(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec)
{
    format_(os, hashvec);
}


void Formats::standard(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec)
{

    std::string routine_at_thread = "Thread: " /*+ std::to_string(tid_)*/;

    // Write headings
    os << "\n";
    os
        << std::setw(40) << std::left  << routine_at_thread  << " "
        << std::setw(15) << std::right << "Self (s)"         << " "
        << std::setw(15) << std::right << "Total (raw) (s)"  << " "
        << std::setw(15) << std::right << "Total (s)"        << " "
        << std::setw(10) << std::right << "Calls"            << "\n";

    os << std::setfill('-');
    os
        << std::setw(40) << "-" << " "
        << std::setw(15) << "-" << " "
        << std::setw(15) << "-" << " "
        << std::setw(15) << "-" << " "
        << std::setw(10) << "-" << "\n";
    os << std::setfill(' ');

    // Data entries
    for (auto& [hash, entry] : hashvec) {
        os
          << std::setw(40) << std::left  << entry.region_name_                << " "
          << std::setw(15) << std::right << entry.self_walltime_.count()      << " "
          << std::setw(15) << std::right << entry.total_raw_walltime_.count() << " "
          << std::setw(15) << std::right << entry.total_walltime_.count()     << " "
          << std::setw(10) << std::right << entry.call_count_                 << "\n";
    }

}

void Formats::drhook(std::ofstream& os, std::vector<std::pair<size_t, HashEntry>> hashvec)
{
    // Preliminary info
    os << "        " << "No. of instrumented routines called : 9\n";
    os << "        " << "Instrumentation started : 20100521 171238\n";
    os << "        " << "Instrumentation   ended : 20100521 172033\n";
    os << "        " << "Instrumentation overhead: 0.90%\n";
    os << "        " << "Memory usage : Memory usage : 1346 MBytes (heap), 1315 MBytes (rss), 796 MBytes (stack), 1116 (paging)\n";
    os << "        " << "Wall-time is 472.28 sec on proc#1 (192 procs, 1 threads)\n";
    os << "        " << "Thread#1:      472.28 sec (100.00%)" << std::endl;
    
    // Table Headers
    os << "\n";
    os 
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
    os
        << "    "
        << std::setw(73) << "" 
        << "(Size; Size/sec; Size/call; MinSize; MaxSize)" << "\n";
    
    // Subheaders
    os 
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
        std::begin(hashvec), std::end(hashvec),
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

    os << std::fixed << std::showpoint << std::setprecision(3);

    for (auto& [hash, entry] : hashvec) {

        // Calculate non-HashEntry data
        region_number++;
        percent_time    = 100.0 * ( entry.self_walltime_.count() / top_walltime );
        cumul_walltime += entry.self_walltime_;
        self_per_call   = 1000.0 * ( entry.self_walltime_.count()  / static_cast<double>(entry.call_count_) );
        total_per_call  = 1000.0 * ( entry.total_walltime_.count() / static_cast<double>(entry.call_count_) );

        // Write everything out 
        os 
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