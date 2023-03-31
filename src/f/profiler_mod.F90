!-------------------------------------------------------------------------------
! (c) Crown copyright 2022 Met Office. All rights reserved.
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
  public :: profiler_get_total_walltime

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
      !> The hash of the region being stopped.
      integer(kind=pik), intent(in) :: hash_in
    end subroutine profiler_stop

    subroutine profiler_write() bind(C, name='c_profiler_write')
        !No arguments to handle
    end subroutine profiler_write

    function profiler_get_total_walltime(hash_in, thread_id) result(walltime) &
             bind(C, name='c_get_total_walltime')
      import :: pik, prk
      integer(kind=pik), intent(in) :: hash_in
      integer(kind=pik), intent(in) :: thread_id
      real(kind=prk)                :: walltime
    end function profiler_get_total_walltime

  end interface

  !-----------------------------------------------------------------------------
  ! Contained functions / subroutines
  !-----------------------------------------------------------------------------
  contains

    !> @brief  Start profiling a code region.
    !> @param [out] hash_out      The unique hash for this region.
    !> @param [in]  region_name   The region name.
    !> @note   Region names need not be null terminated on entry to this
    !>         routine.
    subroutine profiler_start(hash_out, region_name)
      implicit none

      !Arguments
      character(len=*),  intent(in)  :: region_name
      integer(kind=pik), intent(out) :: hash_out

      !Local variables
      character(len=len_trim(region_name)+1) :: local_region_name

      call append_null_char(region_name, local_region_name, len_trim(region_name))

      call interface_profiler_start(hash_out, local_region_name)

    end subroutine profiler_start

    !> @brief  Adds a null character to the end of a string.
    !> @param [in]  strlen      Length of the unterminated string.
    !> @param [in]  string_in   Unterminated string.
    !> @param [out] string_out  Null-terminated string. 
    !> @note  Tests suggested that adding the null character in this manner, as
    !>     opposed to the concatenation operator (//) has performance benefits.
    subroutine append_null_char(string_in, string_out, strlen)
      implicit none

      integer, intent(in)                  :: strlen
      character(len=strlen),   intent(in)  :: string_in
      character(len=strlen+1), intent(out) :: string_out

      string_out(1:strlen)          = string_in(1:strlen)
      string_out(strlen+1:strlen+1) = c_null_char

    end subroutine append_null_char

end module profiler_mod

