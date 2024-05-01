/* -----------------------------------------------------------------------------
*  (c) Crown copyright 2021 Met Office. All rights reserved.
*  The file LICENCE, distributed with this code, contains details of the terms
*  under which the code may be used.
* -----------------------------------------------------------------------------
*/

#include "formatter.h"

#include <iomanip>
#include <algorithm>

/**
 * @brief  Formatter constructor
 * @note   Sets the output format based on the value of environment variable
 *         PROF_OUTPUT_FORMAT.
 */

meto::Formatter::Formatter()
{

  std::string format = "drhook"; 

  char const* env_format = std::getenv("VERNIER_OUTPUT_FORMAT");
  if(env_format){ format = env_format; }

  if ( format == "threads") 
  {
      format_ = &Formatter::threads;
  }
  else if (format == "drhook")
  {
      format_ = &Formatter::drhook;
  }
  else
  {
    std::string error_msg = "Invalid Vernier output format choice. Expected 'threads' or 'drhook'. Currently set to '"
                            + format
                            + "'.";
    throw std::runtime_error( error_msg );
  }
}

/**
 * @brief  Executes the format_ method to write the data.
 * 
 * @param[in] os       Output stream that the format method will write to
 * @param[in] hashvec  Vector of data that the format method will operate on
 */

void meto::Formatter::execute_format(std::ofstream& os, hashvec_t hashvec) 
{
    (this->*format_)(os, hashvec);
}

/**
 * @brief  Per-thread timing output.
 * 
 * @param[in] os       Output stream to write to
 * @param[in] hashvec  Vector containing all the necessary data
 */

void meto::Formatter::threads(std::ofstream& os, hashvec_t hashvec)
{

  // Write key
  os << "\n";
  os << "region_name@thread_id" << "\n"
     << "Self time : Time accrued by region itself. (Exclusive time.)" << "\n"
     << "Total time: Time including cost of child routines and profiling overheads. (Inclusive time.)" << "\n"
     << "Overhead  : Profiling overhead incurred through direct child routine calls only." << "\n"
     << "Calls     : Number of times the region is called." << "\n";

  // Write headings
  os << "\n";
  os
      << std::setw(40) << std::left  << "Region"           << " "
      << std::setw(15) << std::right << "Self (s)"         << " "
      << std::setw(15) << std::right << "Total (s)"        << " "
      << std::setw(15) << std::right << "Overhead (s)"     << " "
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
  for (auto const& record : hashvec) {
      os
        << std::setw(40) << std::left  << record.decorated_region_name_     << " "
        << std::setw(15) << std::right << record.self_walltime_.count()     << " "
        << std::setw(15) << std::right << record.total_walltime_.count()    << " "
        << std::setw(15) << std::right << record.overhead_walltime_.count() << " "
        << std::setw(10) << std::right << record.call_count_                << "\n";
  }

}

/**
 * @brief  Drhook-style output format, important for using the same post
 *         processing tools
 * 
 * @param[in] os       Output stream to write to
 * @param[in] hashvec  Vector containing all the necessary data
 */

void meto::Formatter::drhook(std::ofstream& os, hashvec_t hashvec)
{

  int num_threads = 1;
#ifdef _OPENMP
  num_threads = omp_get_max_threads();
#endif

  // Preliminary info
  os << "Profiling on " << num_threads << " thread(s).\n";
  
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
      return a.total_walltime_ < b.total_walltime_; 
      } 
  )->total_walltime_.count(); 

  // Declare any variables external to RegionRecord
  int             region_number = 0;
  double          percent_time;
  time_duration_t cumul_walltime = time_duration_t::zero();
  double          self_per_call;
  double          total_per_call;
  
  // 
  // Write data to file
  // 

  os << std::fixed << std::showpoint << std::setprecision(3);

  for (auto const& record : hashvec) {

    // Calculate non-RegionRecord data
    region_number++;
    percent_time    = 100.0 * ( record.self_walltime_.count() / top_walltime );
    cumul_walltime += record.self_walltime_;
    self_per_call   = 1000.0 * ( record.self_walltime_.count()  / static_cast<double>(record.call_count_) );
    total_per_call  = 1000.0 * ( record.total_walltime_.count() / static_cast<double>(record.call_count_) );

    // Write everything out 
    os
      << "    "
      << std::setw(3)  << std::left  << region_number
      << std::setw(7)  << std::right << percent_time
      << std::setw(13) << std::right << cumul_walltime.count()
      << std::setw(13) << std::right << record.self_walltime_.count()
      << std::setw(13) << std::right << record.total_walltime_.count()
      << std::setw(15) << std::right << record.call_count_
      << std::setw(12) << std::right << self_per_call
      << std::setw(12) << std::right << total_per_call                 << "    "
                                     << record.decorated_region_name_  << "\n";
  }

}
