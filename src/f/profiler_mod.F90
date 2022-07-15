!-------------------------------------------------------------------------------
! (c) Crown copyright 2022 Met Office. All rights reserved.
!
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

module profiler_mod
  use, intrinsic :: iso_c_binding, only: c_char, c_long, c_double
  implicit none
  private

  !------------------------------------------------------------------------------
  ! Public parameters
  !------------------------------------------------------------------------------

  integer, public, parameter :: pik = c_long
  integer, public, parameter :: prk = c_double
  
  !------------------------------------------------------------------------------
  ! Public interfaces
  !------------------------------------------------------------------------------

  public :: profiler_start
  public :: profiler_stop
  public :: profiler_write
  public :: profiler_get_total_wallclock_time

  interface

    subroutine profiler_start(hash_out, name)  bind(C, name='c_profiler_start')
          import :: c_char, c_long
          character(kind=c_char, len=1), intent(in)  :: name
          integer(kind=c_long),          intent(out) :: hash_out
    end subroutine profiler_start

    subroutine profiler_stop(hash_in) bind(C, name='c_profiler_stop')
        import :: c_long
        integer(kind=c_long), intent(in) :: hash_in
    end subroutine profiler_stop

    subroutine profiler_write() bind(C, name='c_profiler_write')
        !No arguments to handle
    end subroutine profiler_write

    real(kind=c_double) function profiler_get_total_wallclock_time() &
        bind(C, name='c_get_total_wallclock_time')
        import :: c_double
    end function profiler_get_total_wallclock_time

  end interface

end module profiler_mod

