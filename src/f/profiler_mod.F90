!-------------------------------------------------------------------------------
! (c) Crown copyright 2022 Met Office. All rights reserved.
!
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

!> @file    profiler_mod.F90
!> @brief   Provides Fortran profiler bindings.

module profiler_mod
  use, intrinsic :: iso_c_binding, only: c_char, c_long, c_double, c_null_char
  implicit none
  private

  !-----------------------------------------------------------------------------
  ! Public parameters
  !-----------------------------------------------------------------------------

  !> The integer kind for region hashes.
  integer, public, parameter :: pik = c_long

  !> The real kind for region timings.
  integer, public, parameter :: prk = c_double
  
  !-----------------------------------------------------------------------------
  ! Public interfaces / subroutines
  !-----------------------------------------------------------------------------

  public :: profiler_start
  public :: profiler_stop
  public :: profiler_write
  public :: profiler_get_thread0_walltime

  !-----------------------------------------------------------------------------
  ! Interfaces
  !-----------------------------------------------------------------------------

  interface

    subroutine interface_profiler_start(hash_out, region_name)         &
               bind(C, name='c_profiler_start')
      import :: c_char, pik
      character(kind=c_char, len=1), intent(in)  :: region_name(*)
      integer(kind=pik),             intent(out) :: hash_out
    end subroutine interface_profiler_start

    subroutine profiler_stop(hash_in) bind(C, name='c_profiler_stop')
      import :: pik
      integer(kind=pik), intent(in) :: hash_in
    end subroutine profiler_stop

    subroutine profiler_write() bind(C, name='c_profiler_write')
        !No arguments to handle
    end subroutine profiler_write

    function profiler_get_thread0_walltime(hash_in) result(walltime)   &
             bind(C, name='c_get_thread0_walltime')
      import :: pik, prk
      integer(kind=pik), intent(in) :: hash_in
      real(kind=prk)                :: walltime
    end function profiler_get_thread0_walltime

  end interface

  !-----------------------------------------------------------------------------
  ! Contained functions / subroutines
  !-----------------------------------------------------------------------------
  contains

    !> @brief  Start profiling a code region.
    !> @param [out] hash_out      The unique hash for this region.
    !> @param [in]  region_name   The region name.
    !> @note   Routine names need not be null terminated:
    !>         this routine will add a null termination character.
    subroutine profiler_start(hash_out, region_name)
      implicit none

      !Arguments
      character(len=*),  intent(in)  :: region_name
      integer(kind=pik), intent(out) :: hash_out

      !Local variables
      character(len=len_trim(region_name)+1) :: local_region_name
     
      local_region_name = trim(region_name) // c_null_char
      call interface_profiler_start(hash_out, local_region_name)

    end subroutine profiler_start

end module profiler_mod

