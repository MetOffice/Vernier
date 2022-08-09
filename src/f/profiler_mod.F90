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

  !> @defgroup FortranAPI
  !> @brief Fortran API
  public :: profiler_start
  public :: profiler_stop
  public :: profiler_write
  public :: profiler_get_thread0_walltime

  !-----------------------------------------------------------------------------
  ! Interfaces
  !-----------------------------------------------------------------------------

  interface
	!> @ingroup FortranAPI
	!> @fn profiler_mod::profiler_stop::profiler_stop(hash_in)
	!> @brief Stop profiler
	!> @param [in] hash_in The hash of the region to be stopped.
    subroutine profiler_stop(hash_in) bind(C, name='c_profiler_stop')
      import :: pik
      integer(kind=pik), intent(in) :: hash_in
    end subroutine profiler_stop

    !> @ingroup FortranAPI
	!> @fn profiler_mod::profiler_write::profiler_write()
	!> @brief Write profiling data out
    subroutine profiler_write() bind(C, name='c_profiler_write')
        !No arguments to handle
    end subroutine profiler_write

    !> @ingroup FortranAPI
	!> @fn profiler_mod::profiler_get_thread0_walltime::profiler_get_thread0_walltime(hash_in)
	!> @brief Write profiling data out
	!> @param [in] hash_in The hash of the region to return the time.
	!> @returns The Walltime within the region.
    function profiler_get_thread0_walltime(hash_in) result(walltime)   &
             bind(C, name='c_get_thread0_walltime')
      import :: pik, prk
      integer(kind=pik), intent(in) :: hash_in
      real(kind=prk)                :: walltime
    end function profiler_get_thread0_walltime
    
	!> @}
  end interface
  
  private 
  interface
    subroutine interface_profiler_start(hash_out, region_name)                   &
               bind(C, name='c_profiler_start')
      import :: c_char, pik
      character(kind=c_char, len=1), intent(in)  :: region_name
      integer(kind=pik),             intent(out) :: hash_out
    end subroutine interface_profiler_start
  end interface  

  !-----------------------------------------------------------------------------
  ! Contained functions / subroutines
  !-----------------------------------------------------------------------------
  contains

	!> @ingroup FortranAPI
    !> @brief  Start a profiled region, taking care to add a C null character to
    !>         the end of the region name.
    !> @param [out] hash_out      The unique hash for this region.
    !> @param [in]  region_name   The region name.
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

