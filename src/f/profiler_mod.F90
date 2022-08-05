!-------------------------------------------------------------------------------
! (c) Crown copyright 2022 Met Office. All rights reserved.
!
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

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
    subroutine intf_profiler_start(hash_out, region)                   &
               bind(C, name='c_profiler_start')
      import :: c_char, pik
      character(kind=c_char, len=1), intent(in)  :: region
      integer(kind=pik),             intent(out) :: hash_out
    end subroutine intf_profiler_start

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

    !> @brief  Start a profiled region, taking care to add a C null character to
    !>         the end of the region name.
    !> @param [out] hash_out  The unique hash for this region.
    !> @param [in]  region    The region name.
    subroutine profiler_start(hash_out, region)
      implicit none

      !Arguments
      character(len=*),  intent(in)  :: region
      integer(kind=pik), intent(out) :: hash_out

      !Local variables
      character(len=len_trim(region)+1) :: local_region
     
      local_region = trim(region) // c_null_char
      call intf_profiler_start(hash_out, local_region)

    end subroutine profiler_start

end module profiler_mod

