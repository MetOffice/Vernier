/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashtable.h"
#include <iomanip>
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

/**
 * @brief  Constructs a new entry in the hash table.
 *
 */

HashEntry::HashEntry(std::string_view region_name)
      : region_name_(region_name)
      , total_walltime_(time_duration_t::zero())
      , self_walltime_(time_duration_t::zero())
      , child_walltime_(time_duration_t::zero())
      , call_count_(0)
      {}

/**
 * @brief Hashtable constructor
 *
 */

HashTable::HashTable(int const tid)
  : tid_(tid)
  {}

/**
 * @brief  Inserts a new entry into the hashtable.
 *
 */

size_t HashTable::query_insert(std::string_view region_name) noexcept
{
  size_t hash = hash_function_(region_name);

  if (table_.count(hash) == 0){
    table_.emplace(hash,HashEntry(region_name));
    assert (table_.count(hash) > 0);
  }

  return hash;
}

/**
 * @brief  Updates entries in the hashtable.
 *
 */

void HashTable::update(size_t hash, time_duration_t time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.total_walltime_ += time_delta;

  // Update the number of times this region has been called
  entry.call_count_++;
}

/**
 * @brief  Add child time to parent, and recompute the self time.
 *
 */

void HashTable::add_child_time(size_t hash, time_duration_t time_delta)
{
  // Assertions
  assert (table_.size() > 0);
  assert (table_.count(hash) > 0);

  // Increment the walltime for this hash entry
  auto& entry = table_.at(hash);
  entry.child_walltime_ += time_delta;
}

/**
 * @brief  Writes all entries in the hashtable, sorted according to self times.
 *
 */

void HashTable::write()
{

  this->compute_self_times();

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

  // Create a vector from the hashtable and sort the entries according to self
  // walltime.  If optimisation of this is needed, it ought to be possible to
  // acquire a vector of hash-selftime pairs in the correct order, then use the
  // hashes to look up other information directly from the hashtable.
  auto hashvec = std::vector<std::pair<size_t, HashEntry>>(table_.cbegin(), table_.cend());
  std::sort(begin(hashvec), end(hashvec),
      [](auto a, auto b) { return a.second.self_walltime_ > b.second.self_walltime_; });

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

  std::cout << std::fixed << std::showpoint << std::setprecision(3);

  for (auto& [hash, entry] : hashvec) {

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

/**
 * @brief  Combines the table_ of two HashTable's together.
 * 
 * @param[in] ht  The input HashTable
 */

void HashTable::combine(const HashTable& ht)
{
  table_.insert(ht.table_.begin(), ht.table_.end());
}

/**
 * @brief  Computes self times from total times.
 *
 */

 void HashTable::compute_self_times()
 {
   for (auto& [hash, entry] : table_) {
     entry.self_walltime_ = entry.total_walltime_ - entry.child_walltime_;
   }
 }

/**
 * @brief  Produce a list of keys stored in the hashtable.
 *
 */

std::vector<size_t> HashTable::list_keys()
{
  std::vector<size_t> keys;
  for (auto const& key : table_)
  {
    keys.push_back(key.first);
  }
  return keys;
}

/**
 * @brief  Get the total (inclusive) time corresponding to the input hash.
 *
 */

double HashTable::get_total_walltime(size_t const hash) const
{
  return table_.at(hash).total_walltime_.count();
}

/**
 * @brief  Get the profiler self time corresponding to the input hash.
 *
 */

double HashTable::get_self_walltime(size_t const hash)
{
  this->compute_self_times();
  return table_.at(hash).self_walltime_.count();
}

/**
 * @brief  Get the child time corresponding to the input hash.
 *
 */

double HashTable::get_child_walltime(size_t const hash) const
{
  return table_.at(hash).child_walltime_.count();
}

/**
 * @brief  Get the region name corresponding to the input hash.
 *
 */

std::string HashTable::get_region_name(size_t const hash) const
{
  return table_.at(hash).region_name_;
}

 /**
  * @brief  Get the number of times the input hash region has been called.
  *
  * @param[in] hash  The hash corresponding to the region of interest.
  *
  * @returns  Returns an integer corresponding to the number of the times the
  *           region of interest has been called within the code being profiled.
  *
  */

unsigned long long int HashTable::get_call_count(size_t const hash) const
{
    return table_.at(hash).call_count_;
}
