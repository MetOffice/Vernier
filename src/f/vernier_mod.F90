!-------------------------------------------------------------------------------
! (c) Crown copyright 2024 Met Office. All rights reserved.
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

!> @file    vernier_mod.F90
!> @brief   Provides Fortran Vernier bindings.

module vernier_mod
  use, intrinsic :: iso_c_binding, only: c_char, c_long, c_double, c_null_char
  implicit none
  private

  !-----------------------------------------------------------------------------
  ! Public parameters
  !-----------------------------------------------------------------------------

  !> The integer kind for region hashes.
  integer, public, parameter :: vik = c_long

  !> The real kind for region timings.
  integer, public, parameter :: vrk = c_double

  !-----------------------------------------------------------------------------
  ! Public interfaces / subroutines
  !-----------------------------------------------------------------------------

  public :: vernier_init
  public :: vernier_finalize
  public :: vernier_start
  public :: vernier_stop
  public :: vernier_write
  public :: vernier_get_total_walltime
  public :: vernier_get_wtime

  !-----------------------------------------------------------------------------
  ! Interfaces
  !-----------------------------------------------------------------------------

  interface

    subroutine interface_vernier_init(client_comm_handle, tag)  &
               bind(C, name='c_vernier_init')
      import :: c_char
      integer,                       optional, intent(in) :: client_comm_handle
      character(kind=c_char, len=1), optional, intent(in) :: tag(*)
    end subroutine interface_vernier_init

    subroutine vernier_finalize() bind(C, name='c_vernier_finalize')
        !No arguments to handle
    end subroutine vernier_finalize

    subroutine interface_vernier_start_part1()  &
               bind(C, name='c_vernier_start_part1')
        !No arguments to handle
    end subroutine interface_vernier_start_part1

    subroutine interface_vernier_start_part2(hash_out, region_name) &
               bind(C, name='c_vernier_start_part2')
      import :: c_char, vik
      character(kind=c_char, len=1), intent(in)  :: region_name(*)
      integer(kind=vik),             intent(out) :: hash_out
    end subroutine interface_vernier_start_part2

    subroutine vernier_stop(hash_in) bind(C, name='c_vernier_stop')
      import :: vik
      !> The hash of the region being stopped.
      integer(kind=vik), intent(in) :: hash_in
    end subroutine vernier_stop

    subroutine vernier_write() bind(C, name='c_vernier_write')
        !No arguments to handle
    end subroutine vernier_write

    function vernier_get_total_walltime(hash_in, thread_id) result(walltime) &
             bind(C, name='c_vernier_get_total_walltime')
      import :: vik, vrk
      integer(kind=vik), intent(in) :: hash_in
      integer(kind=vik), intent(in) :: thread_id
      real(kind=vrk)                :: walltime
    end function vernier_get_total_walltime

    function vernier_get_wtime() bind(C, name='c_vernier_get_wtime') result(wtime)
      import :: vrk
      real(kind=vrk) :: wtime
    end function vernier_get_wtime

  end interface

  !-----------------------------------------------------------------------------
  ! Contained functions / subroutines
  !-----------------------------------------------------------------------------
  contains

    !> @brief  Initialises Vernier.
    !> @param [in]  client_comm_handle  Handle for the MPI communicator
    !>                                  over which Vernier will operate.
    !> @param [in] tag The tag to appear in the Vernier output filename.
    !> @note   Region names need not be null terminated on entry to this
    !>         routine.
    subroutine vernier_init(client_comm_handle, tag)
      implicit none

      !Arguments
      integer,          optional, intent(in) :: client_comm_handle
      character(len=*), optional, intent(in) :: tag

      !Local variables
      character(len=:), allocatable :: local_tag

      if (present(tag)) then
        allocate(character(len=len_trim(tag)+1) :: local_tag)
        call append_null_char(tag, local_tag, len_trim(tag))
      end if

      call interface_vernier_init(client_comm_handle, local_tag)

    end subroutine vernier_init

    !> @brief  Start profiling a code region.
    !> @param [out] hash_out      The unique hash for this region.
    !> @param [in]  region_name   The region name.
    !> @note   Region names need not be null terminated on entry to this
    !>         routine.
    subroutine vernier_start(hash_out, region_name)
      implicit none

      !Arguments
      character(len=*),  intent(in)  :: region_name
      integer(kind=vik), intent(out) :: hash_out

      !Local variables
      character(len=len_trim(region_name)+1) :: local_region_name

      call interface_vernier_start_part1()

      call append_null_char(region_name, local_region_name, len_trim(region_name))

      call interface_vernier_start_part2(hash_out, local_region_name)

    end subroutine vernier_start

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

end module vernier_mod

