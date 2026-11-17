/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <fstream>
#include <iomanip>
#include <memory>
#include <sched.h>
#include <sstream>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "affinity.h"
#include "error_handler.h"
#include "vernier_mpi.h"

/**
 * @brief Affinity constructor, defaulting to real (machine) system calls.
 */

meto::Affinity::Affinity()
    : system_calls_(std::make_unique<meto::MachineAffinitySysCalls>()) {}

/**
 * @brief Affinity constructor, using the provided system calls strategy object.
 * @param [in] System calls strategy object. (Ownership acquired by
 * constructor.)
 */

meto::Affinity::Affinity(std::unique_ptr<meto::AffinitySysCalls> system_calls)
    : system_calls_(std::move(system_calls)) {}

/**
 * @brief Writes affinitisation map in ASCII-art form.
 * @param [in] mpi_context  Vernier MPI context object.
 * @param [in] fname        Writes file of this name.
 */

void meto::Affinity::write_map(meto::MPIContext const &mpi_context,
                               std::string const &fname) {

  // Maximum number of logical cores on a node.
  std::size_t max_cpus =
      static_cast<std::size_t>(system_calls_->max_available_cpus());

  // Build the header on all ranks for now.
  std::ostringstream hss;
  hss << "--> AFFINITY MAP <--" << "\n\n"
      << "Maximum number of (logical) cores: " << max_cpus << "\n\n"
      << "Thread binding map, key:" << "\n\n"
      << "    . = No threads running on this core.\n"
      << "    # = Multiple threads running on this core.\n"
      << "  0-9 = Threads 0-9.\n"
      << "  a-z = Threads 10-35.\n"
      << "  A-Z = Threads 36-61.\n"
      << "    $ = Threads 62 and greater.\n\n"
      << "MPI rank : ...THREADS..ON..CORES... : "
      << "Num. cores available for migration." << "\n\n"
      << std::string(11, ' ') << "Cores ---->" << "\n";
  std::string header = hss.str();

  // Construct the mask for each rank.
  std::string mask(max_cpus, '.');
#pragma omp parallel default(none) shared(mask, max_cpus)
  {

    // Get the thread ID.
    int thread_id = 0;
#ifdef _OPENMP
    thread_id = omp_get_thread_num();
#endif
    char thread_id_char = thread_id_to_char(thread_id);

    // Get the core ID, and check that it's not out of range.
    auto core_id = static_cast<std::size_t>(system_calls_->running_on_core());
    if (core_id >= max_cpus) {
      meto::error_handler("Affinity::Affinity. Core ID out-of-range.",
                          EXIT_FAILURE);
    }

// If more than one thread is running on the same core, show that with a
// hash symbol.
#pragma omp critical
    {
      if (mask.at(core_id) == '.') {
        mask.at(core_id) = thread_id_char;
      } else {
        mask.at(core_id) = '#';
      }
    } // critical
  } // parallel

  // Construct the record on each individual rank.
  int num_cpus = system_calls_->num_available_cpus();
  std::ostringstream rss;
  rss << std::setw(8) << std::setfill('0') << mpi_context.get_rank() << " : "
      << mask << " : " << std::setw(3) << std::setfill(' ') << num_cpus << "\n";

#ifdef USE_VERNIER_MPI_STUB

  // Rather than dummy out all the MPI calls, replace with a simple file open
  // and write when running without MPI.

  std::ofstream os(fname);

  os << header;
  os << rss.str();

  os.flush();
  os.close();

#else // USE_VERNIER_MPI_STUB

  // Find the maximum record length across MPI ranks.
  int max_record_length;
  int record_length = static_cast<int>(rss.str().length());
  MPI_Allreduce(&record_length, &max_record_length, 1, MPI_INT, MPI_MAX,
                mpi_context.get_handle());

  // Having found the maximum record length, pad out the record on each
  // individual MPI rank with spaces.
  rss << std::string(
      static_cast<std::string::size_type>(max_record_length - record_length),
      ' ');

  MPI_Datatype mpi_buffer;
  MPI_Type_contiguous(max_record_length, MPI_CHAR, &mpi_buffer);
  MPI_Type_commit(&mpi_buffer);

  MPI_Offset header_length = static_cast<MPI_Offset>(header.length());

  // Collective file open
  MPI_File mapfile;
  MPI_File_open(mpi_context.get_handle(), fname.c_str(),
                MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &mapfile);

  // Root writes the header at offset 0
  if (mpi_context.on_root()) {
    MPI_File_write_at(mapfile, 0, header.data(),
                      static_cast<int>(header.size()), MPI_CHARACTER,
                      MPI_STATUS_IGNORE);
  }

  // Each rank computes its own offset.
  MPI_Offset my_offset =
      header_length + (static_cast<MPI_Offset>(mpi_context.get_rank()) *
                       max_record_length * static_cast<int>(sizeof(char)));

  // Create a view for each task which represents a unique, non-overlapping
  // region.
  MPI_File_set_view(mapfile, my_offset, MPI_CHAR, mpi_buffer, "native",
                    MPI_INFO_NULL);

  // Each rank writes its own record.
  MPI_File_write(mapfile, rss.str().c_str(), max_record_length, MPI_CHAR,
                 MPI_STATUS_IGNORE);

  // Close the file collectively.
  MPI_File_close(&mapfile);
  MPI_Type_free(&mpi_buffer);

#endif // USE_VERNIER_MPI_STUB
}

/**
 * @brief Produces a single character indicating the thread number.
 *
 * @details Only the first 62 threads are representable, given the constraint of
 *          numerical digits, lower case letters and uppercase letters. That
 *          should be enough to determine whether affinitisation is working as
 *          expected. Threads with IDs higher than 62 appear as '$'.
 */

char meto::Affinity::thread_id_to_char(int num) {

  char thread_id_char;

  if (num < 10) {
    thread_id_char = static_cast<char>('0' + num);
  } //  10 numerical digits
  else if (num < 36) {
    thread_id_char = static_cast<char>('a' + (num - 10));
  } // +26 lowercase letters
  else if (num < 62) {
    thread_id_char = static_cast<char>('A' + (num - 36));
  } // +26 uppercase letters
  else {
    thread_id_char = '$';
  }

  return thread_id_char;
}

/**
 * @brief  Returns the maximum number of logical cores (real and virtual)
 *         available on a node.
 * @returns  See brief.
 */

int meto::MachineAffinitySysCalls::max_available_cpus() const {
  int max_cpus = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
  if (max_cpus < 0) {
    meto::error_handler(
        "MachineAffinitySysCalls::max_available_cpus: sysconf failed.",
        EXIT_FAILURE);
  }
  return max_cpus;
}

/**
 * @brief  Returns the number of cores (real and virtual) which this thread
 *         may migrate across.
 * @notes  This number will be unity if a thread is bound to a single (logical)
 *         core.
 */

int meto::MachineAffinitySysCalls::num_available_cpus() const {
  {
    int num_cpus;
    cpu_set_t cpumask;
    pid_t tid;

    tid = static_cast<pid_t>(syscall(SYS_gettid));
    CPU_ZERO(&cpumask);

    if (sched_getaffinity(tid, sizeof(cpu_set_t), &cpumask) < 0) {
      meto::error_handler("MachineAffinitySysCalls::num_available_cpus: "
                          "sched_getaffinity failed.",
                          EXIT_FAILURE);
    }

    num_cpus = cpumask_weight(&cpumask);

    return num_cpus;
  }
}

/**
 * @brief    Count the number of elements of a cpumask that are set.
 * @returns  The number of mask elements set.
 */

int meto::MachineAffinitySysCalls::cpumask_weight(cpu_set_t *cpumask) const {
  int weight;
  int index;

  weight = 0;
  for (index = 0; index < CPU_SETSIZE; ++index) {
    if (CPU_ISSET(static_cast<size_t>(index), cpumask)) {
      weight++;
    }
  }

  return weight;
}

/**
 * @brief   Returns the core ID on which the calling thread is running at the
 *          point this method is called.
 * @notes   The layout of core IDs will be system-dependent.
 * @returns The core ID.
 */

int meto::MachineAffinitySysCalls::running_on_core() const {
  int core = sched_getcpu();
  if (core < 0) {
    meto::error_handler(
        "MachineAffinitySysCalls::running_on_core: sched_getcpu failed.",
        EXIT_FAILURE);
  }
  return core;
}
