/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   affinity.h
 * @brief  Classes for affinity reporting and for associated system calls.
 */

#ifndef AFFINITY_H
#define AFFINITY_H

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <memory>
#include <sched.h>
#include <string>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "mpi_context.h"

namespace meto {

// Forward declarations
class AffinitySysCalls;

/**
 * @brief  Affinity reporting class.
 *
 * Class providing methods to interrogate the system and report where threads
 * are running.
 */

class Affinity {

private:
  // Data members
  std::unique_ptr<AffinitySysCalls> system_calls_;

public:
  // Constructors
  Affinity();
  explicit Affinity(std::unique_ptr<meto::AffinitySysCalls>);

  char thread_id_to_char(int);
  void write_map(meto::MPIContext const &, std::string const &);
};

/**
 * @brief  Affinity system call class.
 *
 * Base class to abstract away system calls for affinity reporting. This
 * abstraction is useful for mocking and testing.
 */

class AffinitySysCalls {

public:
  virtual ~AffinitySysCalls() = default;

  virtual int max_available_cpus() const = 0;
  virtual int num_available_cpus() const = 0;
  virtual int running_on_core() const = 0;
};

/**
 * @brief  Affinity system call class.
 *
 * Real (machine) implementation of the affinity system call base class.
 */

class MachineAffinitySysCalls : public AffinitySysCalls {

private:
  int cpumask_weight(cpu_set_t *) const;

public:
  int max_available_cpus() const override;
  int num_available_cpus() const override;
  int running_on_core() const override;
};

} // namespace meto

#endif
